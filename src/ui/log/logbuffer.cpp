#include "logbuffer.h"

#include "fortcommon.h"
#include "logentryblocked.h"
#include "logentryprocnew.h"
#include "logentrystattraf.h"

LogBuffer::LogBuffer(int bufferSize, QObject *parent) :
    QObject(parent),
    m_top(0),
    m_offset(0),
    m_array(bufferSize ? bufferSize : FortCommon::bufferSize(),
            Qt::Uninitialized)
{
}

void LogBuffer::reset(int top)
{
    m_top = top;
    m_offset = 0;
}

char *LogBuffer::output()
{
    return m_array.data() + m_top;
}

const char *LogBuffer::input() const
{
    return m_array.constData() + m_offset;
}

void LogBuffer::prepareFor(int len)
{
    const int newSize = m_top + len;

    if (newSize > m_array.size()) {
        m_array.resize(newSize);
    }
}

LogEntry::LogType LogBuffer::peekEntryType()
{
    if (m_offset >= m_top)
        return LogEntry::TypeNone;

    const char *input = this->input();

    const quint32 type = FortCommon::logType(input);

    return static_cast<LogEntry::LogType>(type);
}

void LogBuffer::writeEntryBlocked(const LogEntryBlocked *logEntry)
{
    const QString path = logEntry->kernelPath();
    const int pathLen = path.size() * sizeof(wchar_t);

    const int entrySize = FortCommon::logBlockedSize(pathLen);
    prepareFor(entrySize);

    char *output = this->output();

    FortCommon::logBlockedHeaderWrite(output, logEntry->ip(),
                                      logEntry->pid(), pathLen);
    output += FortCommon::logBlockedHeaderSize();

    if (pathLen) {
        path.toWCharArray((wchar_t *) output);
    }

    m_top += entrySize;
}

void LogBuffer::readEntryBlocked(LogEntryBlocked *logEntry)
{
    Q_ASSERT(m_offset < m_top);

    const char *input = this->input();

    quint32 ip, pid, pathLen;
    FortCommon::logBlockedHeaderRead(input, &ip, &pid, &pathLen);

    QString path;
    if (pathLen) {
        input += FortCommon::logBlockedHeaderSize();
        path = QString::fromWCharArray((const wchar_t *) input,
                                       pathLen / sizeof(wchar_t));
    }

    logEntry->setIp(ip);
    logEntry->setPid(pid);
    logEntry->setKernelPath(path);

    const int entrySize = FortCommon::logBlockedSize(pathLen);
    m_offset += entrySize;
}

void LogBuffer::writeEntryProcNew(const LogEntryProcNew *logEntry)
{
    const QString path = logEntry->kernelPath();
    const int pathLen = path.size() * sizeof(wchar_t);

    const int entrySize = FortCommon::logProcNewSize(pathLen);
    prepareFor(entrySize);

    char *output = this->output();

    FortCommon::logProcNewHeaderWrite(output, logEntry->pid(), pathLen);
    output += FortCommon::logProcNewHeaderSize();

    if (pathLen) {
        path.toWCharArray((wchar_t *) output);
    }

    m_top += entrySize;
}

void LogBuffer::readEntryProcNew(LogEntryProcNew *logEntry)
{
    Q_ASSERT(m_offset < m_top);

    const char *input = this->input();

    quint32 pid, pathLen;
    FortCommon::logProcNewHeaderRead(input, &pid, &pathLen);

    QString path;
    if (pathLen) {
        input += FortCommon::logProcNewHeaderSize();
        path = QString::fromWCharArray((const wchar_t *) input,
                                       pathLen / sizeof(wchar_t));
    }

    logEntry->setPid(pid);
    logEntry->setKernelPath(path);

    const int entrySize = FortCommon::logProcNewSize(pathLen);
    m_offset += entrySize;
}

void LogBuffer::readEntryStatTraf(LogEntryStatTraf *logEntry)
{
    Q_ASSERT(m_offset < m_top);

    const char *input = this->input();

    quint16 procCount;
    FortCommon::logStatTrafHeaderRead(input, &procCount);

    logEntry->setProcCount(procCount);

    if (procCount) {
        input += FortCommon::logStatHeaderSize();
        logEntry->setProcTrafBytes((const quint32 *) input);
    }

    const int entrySize = FortCommon::logStatTrafSize(procCount);
    m_offset += entrySize;
}
