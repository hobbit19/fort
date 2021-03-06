#include "osutil.h"

#include <QDir>
#include <QProcess>

#define WIN32_LEAN_AND_MEAN
#include <qt_windows.h>

#include "processinfo.h"

OsUtil::OsUtil(QObject *parent) :
    QObject(parent)
{
}

QString OsUtil::pidToPath(quint32 pid, bool isKernelPath)
{
    const ProcessInfo pi(pid);
    return pi.path(isKernelPath);
}

void OsUtil::openFolder(const QString &filePath)
{
    const QString nativePath = QDir::toNativeSeparators(filePath);

    QProcess::execute(QString("explorer.exe /select,%1")
                      .arg(nativePath));
}

bool OsUtil::createGlobalMutex(const char *name)
{
    return CreateMutexA(nullptr, FALSE, name)
            && GetLastError() != ERROR_ALREADY_EXISTS;
}

quint32 OsUtil::lastErrorCode()
{
    return GetLastError();
}

QString OsUtil::lastErrorMessage(quint32 errorCode)
{
    LPWSTR buf = nullptr;

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER
                   | FORMAT_MESSAGE_FROM_SYSTEM
                   | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, errorCode, 0,
                   (LPWSTR) &buf, 0, nullptr);

    if (!buf) {
        return QString("System Error %1").arg(errorCode);
    }

    const QString text = QString::fromUtf16((const ushort *) buf).trimmed();
    LocalFree(buf);
    return text;
}
