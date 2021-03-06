#include "driverworker.h"

#include "../fortcommon.h"
#include "../log/logbuffer.h"
#include "../util/device.h"
#include "../util/osutil.h"

DriverWorker::DriverWorker(Device *device, QObject *parent) :
    QObject(parent),
    m_isLogReading(false),
    m_cancelled(false),
    m_aborted(false),
    m_device(device),
    m_logBuffer(nullptr)
{
}

void DriverWorker::run()
{
    do {
        readLog();
    } while (!m_aborted);
}

bool DriverWorker::readLogAsync(LogBuffer *logBuffer)
{
    QMutexLocker locker(&m_mutex);

    if (m_logBuffer)
        return false;

    m_cancelled = false;

    m_logBuffer = logBuffer;

    m_waitCondition.wakeOne();

    return true;
}

void DriverWorker::cancelAsyncIo()
{
    QMutexLocker locker(&m_mutex);

    m_cancelled = true;

    if (m_isLogReading) {
        m_device->cancelIo();

        do {
            m_waitCondition.wait(&m_mutex);
        } while (m_isLogReading);
    }
}

void DriverWorker::abort()
{
    if (m_aborted) return;

    m_aborted = true;

    cancelAsyncIo();
    readLogAsync(nullptr);
}

bool DriverWorker::waitLogBuffer()
{
    QMutexLocker locker(&m_mutex);

    while (!m_logBuffer || m_cancelled) {
        if (m_aborted)
            return false;

        m_waitCondition.wait(&m_mutex);
    }

    m_isLogReading = true;

    return true;
}

void DriverWorker::emitReadLogResult(bool success,
                                     const QString &errorMessage)
{
    QMutexLocker locker(&m_mutex);

    m_isLogReading = false;

    LogBuffer *logBuffer = m_logBuffer;
    m_logBuffer = nullptr;

    emit readLogResult(logBuffer, success, errorMessage);

    if (m_cancelled) {
        m_waitCondition.wakeOne();
    }
}

void DriverWorker::readLog()
{
    if (!waitLogBuffer())
        return;

    QByteArray &array = m_logBuffer->array();
    int nr;

    const bool success = m_device->ioctl(
                FortCommon::ioctlGetLog(), nullptr, 0,
                array.data(), array.size(), &nr);

    QString errorMessage;

    if (success) {
        m_logBuffer->reset(nr);
    } else if (!m_cancelled) {
        errorMessage = OsUtil::lastErrorMessage();
    }

    emitReadLogResult(success, errorMessage);
}
