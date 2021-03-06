#include "appinfomanager.h"

#include <QImage>
#include <QLoggingCategory>

#include "../fileutil.h"
#include "appinfojob.h"
#include "appinfoworker.h"
#include "apputil.h"
#include <sqlite/sqlitedb.h>
#include <sqlite/sqlitestmt.h>

Q_DECLARE_LOGGING_CATEGORY(CLOG_APPINFOCACHE)
Q_LOGGING_CATEGORY(CLOG_APPINFOCACHE, "fort.appInfoWorker")

#define DATABASE_USER_VERSION   2

#define APP_CACHE_MAX_COUNT     2000

namespace {

const char * const sqlSelectAppInfo =
        "SELECT file_descr, company_name,"
        "    product_name, product_ver, icon_id"
        "  FROM app WHERE path = ?1;"
        ;

const char * const sqlUpdateAppAccessTime =
        "UPDATE app"
        "  SET access_time = datetime('now')"
        "  WHERE path = ?1;"
        ;

const char * const sqlSelectIconImage =
        "SELECT image FROM icon WHERE icon_id = ?1;"
        ;

const char * const sqlSelectIconIdByHash =
        "SELECT icon_id FROM icon WHERE hash = ?1;"
        ;

const char * const sqlInsertIcon =
        "INSERT INTO icon(ref_count, hash, image)"
        "  VALUES(1, ?1, ?2);"
        ;

const char * const sqlUpdateIconRefCount =
        "UPDATE icon"
        "  SET ref_count = ref_count + ?2"
        "  WHERE icon_id = ?1;"
        ;

const char * const sqlInsertAppInfo =
        "INSERT INTO app(path, file_descr, company_name,"
        "    product_name, product_ver, icon_id, access_time)"
        "  VALUES(?1, ?2, ?3, ?4, ?5, ?6, datetime('now'));"
        ;

const char * const sqlSelectAppCount =
        "SELECT count(*) FROM app;"
        ;

const char * const sqlSelectAppOlds =
        "SELECT path, icon_id"
        "  FROM app"
        "  ORDER BY access_time DESC"
        "  LIMIT ?1;"
        ;

const char * const sqlDeleteIconIfNotUsed =
        "DELETE FROM icon"
        "  WHERE icon_id = ?1 AND ref_count = 0;"
        ;

const char * const sqlDeleteApp =
        "DELETE FROM app WHERE path = ?1;"
        ;

bool migrateFunc(SqliteDb *db, int version, void *ctx)
{
    Q_UNUSED(db)

    AppInfoManager *manager = static_cast<AppInfoManager *>(ctx);
    bool res = true;

    if (version == 2) {
        // Delete "System" app
        const char * const sqlSelectAppSystem =
                "SELECT path, icon_id FROM app"
                "  WHERE path = 'System';"
                ;
        res = manager->deleteApps(sqlSelectAppSystem);
    }

    return res;
}

}

AppInfoManager::AppInfoManager(QObject *parent) :
    WorkerManager(parent),
    m_sqliteDb(new SqliteDb())
{
    setMaxWorkersCount(1);

    setupDb();
}

AppInfoManager::~AppInfoManager()
{
    delete m_sqliteDb;
}

void AppInfoManager::setupDb()
{
    const QString cachePath = FileUtil::appCacheLocation();

    FileUtil::makePath(cachePath);

    const QString filePath = cachePath + "/appinfocache.db";

    if (!m_sqliteDb->open(filePath)) {
        qCritical(CLOG_APPINFOCACHE()) << "File open error:"
                                       << filePath
                                       << m_sqliteDb->errorMessage();
        return;
    }

    if (!m_sqliteDb->migrate(":/appinfocache/migrations", DATABASE_USER_VERSION,
                             &migrateFunc, this)) {
        qCritical(CLOG_APPINFOCACHE()) << "Migration error" << filePath;
        return;
    }
}

WorkerObject *AppInfoManager::createWorker()
{
    return new AppInfoWorker(this);
}

void AppInfoManager::lookupAppInfo(const QString &appPath)
{
    enqueueJob(new AppInfoJob(appPath));
}

void AppInfoManager::handleWorkerResult(WorkerJob *workerJob)
{
    if (!aborted()) {
        auto job = static_cast<AppInfoJob *>(workerJob);

        emit lookupFinished(job->appPath(), job->appInfo);
    }

    delete workerJob;
}

bool AppInfoManager::loadInfoFromFs(const QString &appPath, AppInfo &appInfo)
{
    return AppUtil::getInfo(appPath, appInfo);
}

QImage AppInfoManager::loadIconFromFs(const QString &appPath)
{
    return AppUtil::getIcon(appPath);
}

bool AppInfoManager::loadInfoFromDb(const QString &appPath, AppInfo &appInfo)
{
    QMutexLocker locker(&m_mutex);

    const QVariantList vars = QVariantList() << appPath;

    // Load version info
    const int resultCount = 5;
    const QVariantList list = m_sqliteDb->executeEx(
                sqlSelectAppInfo, vars, resultCount)
            .toList();
    if (list.size() != resultCount)
        return false;

    appInfo.fileDescription = list.at(0).toString();
    appInfo.companyName = list.at(1).toString();
    appInfo.productName = list.at(2).toString();
    appInfo.productVersion = list.at(3).toString();
    appInfo.iconId = list.at(4).toLongLong();

    // Update last access time
    m_sqliteDb->executeEx(sqlUpdateAppAccessTime, vars);

    return true;
}

QImage AppInfoManager::loadIconFromDb(qint64 iconId)
{
    QMutexLocker locker(&m_mutex);

    const QVariant icon = m_sqliteDb->executeEx(
                sqlSelectIconImage, QVariantList() << iconId);

    return icon.value<QImage>();
}

bool AppInfoManager::saveToDb(const QString &appPath, AppInfo &appInfo,
                              const QImage &appIcon)
{
    QMutexLocker locker(&m_mutex);

    bool ok = true;

    m_sqliteDb->beginTransaction();

    // Save icon image
    QVariant iconId;
    {
        const uint iconHash = qHashBits(appIcon.constBits(),
                                        size_t(appIcon.sizeInBytes()));

        iconId = m_sqliteDb->executeEx(sqlSelectIconIdByHash,
                                       QVariantList() << iconHash);
        if (iconId.isNull()) {
            m_sqliteDb->executeEx(sqlInsertIcon,
                                  QVariantList() << iconHash << appIcon,
                                  0, &ok);
            if (ok) {
                iconId = m_sqliteDb->lastInsertRowid();
            }
        } else {
            m_sqliteDb->executeEx(sqlUpdateIconRefCount,
                                  QVariantList() << iconId << +1,
                                  0, &ok);
        }
    }

    // Save version info
    if (ok) {
        const QVariantList vars = QVariantList()
                << appPath
                << appInfo.fileDescription
                << appInfo.companyName
                << appInfo.productName
                << appInfo.productVersion
                << iconId
                   ;

        m_sqliteDb->executeEx(sqlInsertAppInfo, vars, 0, &ok);
    }

    m_sqliteDb->endTransaction(ok);

    if (ok) {
        appInfo.iconId = iconId.toLongLong();

        // Delete excess info
        const int appCount = m_sqliteDb->executeEx(sqlSelectAppCount).toInt();
        const int excessCount = appCount - APP_CACHE_MAX_COUNT;

        if (excessCount > 0) {
            deleteApps(sqlSelectAppOlds, excessCount);
        }
    }

    return ok;
}

bool AppInfoManager::deleteApps(const char *sql, int limitCount)
{
    QStringList appPaths;
    QHash<qint64, int> iconIds;

    bool ok = false;

    m_sqliteDb->beginTransaction();

    // Get old app info list
    {
        SqliteStmt stmt;
        if (stmt.prepare(m_sqliteDb->db(), sql,
                         SqliteStmt::PreparePersistent)) {
            if (limitCount != 0) {
                stmt.bindInt(1, limitCount);
            }

            while (stmt.step() == SqliteStmt::StepRow) {
                const QString appPath = stmt.columnText(0);
                appPaths.append(appPath);

                const qint64 iconId = stmt.columnInt64(1);
                const int iconCount = iconIds.value(iconId) + 1;
                iconIds.insert(iconId, iconCount);
            }

            ok = true;
        }
    }

    // Delete old icons
    auto iconIt = iconIds.constBegin();
    for (; iconIt != iconIds.constEnd(); ++iconIt) {
        const qint64 iconId = iconIt.key();
        const int count = iconIt.value();

        m_sqliteDb->executeEx(sqlUpdateIconRefCount,
                              QVariantList() << iconId << -count,
                              0, &ok);
        if (!ok) goto end;

        m_sqliteDb->executeEx(sqlDeleteIconIfNotUsed,
                              QVariantList() << iconId,
                              0, &ok);
        if (!ok) goto end;
    }

    // Delete old app infos
    for (const QString &path : appPaths) {
        m_sqliteDb->executeEx(sqlDeleteApp, QVariantList() << path,
                              0, &ok);
        if (!ok) goto end;
    }

 end:
    m_sqliteDb->endTransaction(ok);

    return ok;
}
