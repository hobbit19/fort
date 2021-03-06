#include "fileutil.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

#define WIN32_LEAN_AND_MEAN
#include <qt_windows.h>

FileUtil::FileUtil(QObject *parent) :
    QObject(parent)
{
    Q_ASSERT(sizeof(wchar_t) == sizeof(QChar));
}

// Convert "\\Device\\HarddiskVolume1" to "C:"
QString FileUtil::kernelNameToDrive(const QString &kernelName)
{
    const QString kernelNameLower = kernelName.toLower();

    foreach (const QFileInfo &fi, QDir::drives()) {
        const QString driveName = fi.path().left(2);
        const QString driveKernelName = driveToKernelName(driveName);

        if (kernelNameLower == driveKernelName.toLower()) {
            return driveName;
        }
    }
    return QString();
}

// Convert "C:" to "\\Device\\HarddiskVolume1"
QString FileUtil::driveToKernelName(const QString &drive)
{
    char driveName[3] = {drive.at(0).toLatin1(), ':', '\0'};

    char buf[MAX_PATH];
    const int len = QueryDosDeviceA((LPCSTR) driveName,
                                    buf, MAX_PATH);

    return (len > 0) ? QString::fromLatin1(buf)
                     : QString();
}

// Convert "\\Device\\HarddiskVolume1\\path" to "C:\\path"
QString FileUtil::kernelPathToPath(const QString &kernelPath)
{
    const QLatin1Char sep('\\');

    if (kernelPath.startsWith(sep)) {
        const int sepPos1 = kernelPath.indexOf(sep, 1);
        if (sepPos1 > 0) {
            const int sepPos2 = kernelPath.indexOf(sep, sepPos1 + 1);
            if (sepPos2 > 0) {
                const QString kernelName = kernelPath.left(sepPos2);
                return kernelNameToDrive(kernelName) + kernelPath.mid(sepPos2);
            }
        }
    }
    return kernelPath;
}

// Convert "C:\\path" to "\\Device\\HarddiskVolume1\\path"
QString FileUtil::pathToKernelPath(const QString &path)
{
    const QString drive = path.left(2);

    if (drive.at(0).isLetter() && drive.at(1) == QLatin1Char(':')) {
        return driveToKernelName(drive)
                + path.mid(2).replace(QLatin1Char('/'), QLatin1Char('\\'));
    }
    return path;
}

QString FileUtil::fileName(const QString &path)
{
    return QFileInfo(path).fileName();
}

QString FileUtil::absolutePath(const QString &path)
{
    return QDir(path).absolutePath();
}

QString FileUtil::pathSlash(const QString &path)
{
    const QLatin1Char slash('/');
    return path.endsWith(slash) ? path : path + slash;
}

QString FileUtil::toNativeSeparators(const QString &path)
{
    return QDir::toNativeSeparators(path);
}

bool FileUtil::makePath(const QString &path)
{
    return QDir().mkpath(path);
}

bool FileUtil::fileExists(const QString &filePath)
{
    return QFileInfo::exists(filePath);
}

bool FileUtil::removeFile(const QString &filePath)
{
    return QFile::remove(filePath);
}

bool FileUtil::renameFile(const QString &oldFilePath, const QString &newFilePath)
{
    removeFile(newFilePath);

    return QFile::rename(oldFilePath, newFilePath);
}

bool FileUtil::copyFile(const QString &filePath, const QString &newFilePath)
{
    return QFile::copy(filePath, newFilePath);
}

bool FileUtil::linkFile(const QString &filePath, const QString &linkPath)
{
    return QFile::link(filePath, linkPath);
}

QString FileUtil::readFile(const QString &filePath)
{
    return QString::fromUtf8(readFileData(filePath));
}

QByteArray FileUtil::readFileData(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly))
        return QByteArray();

    return file.readAll();
}

bool FileUtil::writeFile(const QString &filePath, const QString &text)
{
    return writeFileData(filePath, text.toUtf8());
}

bool FileUtil::writeFileData(const QString &filePath, const QByteArray &data)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
        return false;

    return file.write(data) == data.size()
            && file.flush();
}

QString FileUtil::appBinLocation()
{
    return QCoreApplication::applicationDirPath();
}

QString FileUtil::appCacheLocation()
{
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
}

QString FileUtil::appConfigLocation()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

QString FileUtil::applicationsLocation()
{
    return QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
}
