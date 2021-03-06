#include "databasesql.h"

const char * const DatabaseSql::sqlPragmas =
        "PRAGMA locking_mode=EXCLUSIVE;"
        "PRAGMA synchronous=NORMAL;"
        ;

const char * const DatabaseSql::sqlSelectAppId =
        "SELECT app_id FROM app WHERE path = ?1;"
        ;

const char * const DatabaseSql::sqlInsertAppId =
        "INSERT INTO app(path, creat_time, traf_time, in_bytes, out_bytes)"
        "  VALUES(?1, ?2, ?3, 0, 0);"
        ;

const char * const DatabaseSql::sqlDeleteAppId =
        "DELETE FROM app WHERE app_id = ?1;"
        ;

const char * const DatabaseSql::sqlSelectAppPaths =
        "SELECT app_id, path FROM app ORDER BY creat_time;"
        ;

const char * const DatabaseSql::sqlInsertTrafAppHour =
        "INSERT INTO traffic_app_hour(app_id, traf_time, in_bytes, out_bytes)"
        "  VALUES(?4, ?1, ?2, ?3);"
        ;

const char * const DatabaseSql::sqlInsertTrafAppDay =
        "INSERT INTO traffic_app_day(app_id, traf_time, in_bytes, out_bytes)"
        "  VALUES(?4, ?1, ?2, ?3);"
        ;

const char * const DatabaseSql::sqlInsertTrafAppMonth =
        "INSERT INTO traffic_app_month(app_id, traf_time, in_bytes, out_bytes)"
        "  VALUES(?4, ?1, ?2, ?3);"
        ;

const char * const DatabaseSql::sqlInsertTrafHour =
        "INSERT INTO traffic_hour(traf_time, in_bytes, out_bytes)"
        "  VALUES(?1, ?2, ?3);"
        ;

const char * const DatabaseSql::sqlInsertTrafDay =
        "INSERT INTO traffic_day(traf_time, in_bytes, out_bytes)"
        "  VALUES(?1, ?2, ?3);"
        ;

const char * const DatabaseSql::sqlInsertTrafMonth =
        "INSERT INTO traffic_month(traf_time, in_bytes, out_bytes)"
        "  VALUES(?1, ?2, ?3);"
        ;

const char * const DatabaseSql::sqlUpdateTrafAppHour =
        "UPDATE traffic_app_hour"
        "  SET in_bytes = in_bytes + ?2,"
        "    out_bytes = out_bytes + ?3"
        "  WHERE app_id = ?4 and traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlUpdateTrafAppDay =
        "UPDATE traffic_app_day"
        "  SET in_bytes = in_bytes + ?2,"
        "    out_bytes = out_bytes + ?3"
        "  WHERE app_id = ?4 and traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlUpdateTrafAppMonth =
        "UPDATE traffic_app_month"
        "  SET in_bytes = in_bytes + ?2,"
        "    out_bytes = out_bytes + ?3"
        "  WHERE app_id = ?4 and traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlUpdateTrafAppTotal =
        "UPDATE app"
        "  SET in_bytes = in_bytes + ?2,"
        "    out_bytes = out_bytes + ?3"
        "  WHERE app_id = ?4 and 0 != ?1;"
        ;

const char * const DatabaseSql::sqlUpdateTrafHour =
        "UPDATE traffic_hour"
        "  SET in_bytes = in_bytes + ?2,"
        "    out_bytes = out_bytes + ?3"
        "  WHERE traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlUpdateTrafDay =
        "UPDATE traffic_day"
        "  SET in_bytes = in_bytes + ?2,"
        "    out_bytes = out_bytes + ?3"
        "  WHERE traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlUpdateTrafMonth =
        "UPDATE traffic_month"
        "  SET in_bytes = in_bytes + ?2,"
        "    out_bytes = out_bytes + ?3"
        "  WHERE traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlSelectMinTrafAppHour =
        "SELECT min(traf_time) FROM traffic_app_hour"
        "  WHERE app_id = ?1;"
        ;

const char * const DatabaseSql::sqlSelectMinTrafAppDay =
        "SELECT min(traf_time) FROM traffic_app_day"
        "  WHERE app_id = ?1;"
        ;

const char * const DatabaseSql::sqlSelectMinTrafAppMonth =
        "SELECT min(traf_time) FROM traffic_app_month"
        "  WHERE app_id = ?1;"
        ;

const char * const DatabaseSql::sqlSelectMinTrafAppTotal =
        "SELECT traf_time FROM app WHERE app_id = ?1;"
        ;

const char * const DatabaseSql::sqlSelectMinTrafHour =
        "SELECT min(traf_time) FROM traffic_hour;"
        ;

const char * const DatabaseSql::sqlSelectMinTrafDay =
        "SELECT min(traf_time) FROM traffic_app_day;"
        ;

const char * const DatabaseSql::sqlSelectMinTrafMonth =
        "SELECT min(traf_time) FROM traffic_app_month;"
        ;

const char * const DatabaseSql::sqlSelectMinTrafTotal =
        "SELECT min(traf_time) FROM app;"
        ;

const char * const DatabaseSql::sqlSelectTrafAppHour =
        "SELECT in_bytes, out_bytes"
        "  FROM traffic_app_hour"
        "  WHERE app_id = ?2 and traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlSelectTrafAppDay =
        "SELECT in_bytes, out_bytes"
        "  FROM traffic_app_day"
        "  WHERE app_id = ?2 and traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlSelectTrafAppMonth =
        "SELECT in_bytes, out_bytes"
        "  FROM traffic_app_month"
        "  WHERE app_id = ?2 and traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlSelectTrafAppTotal =
        "SELECT in_bytes, out_bytes"
        "  FROM app"
        "  WHERE app_id = ?2 and 0 != ?1;"
        ;

const char * const DatabaseSql::sqlSelectTrafHour =
        "SELECT in_bytes, out_bytes"
        "  FROM traffic_hour WHERE traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlSelectTrafDay =
        "SELECT in_bytes, out_bytes"
        "  FROM traffic_day WHERE traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlSelectTrafMonth =
        "SELECT in_bytes, out_bytes"
        "  FROM traffic_month WHERE traf_time = ?1;"
        ;

const char * const DatabaseSql::sqlSelectTrafTotal =
        "SELECT sum(in_bytes), sum(out_bytes)"
        "  FROM app WHERE 0 != ?1;"
        ;

const char * const DatabaseSql::sqlDeleteTrafAppHour =
        "DELETE FROM traffic_app_hour"
        "  WHERE traf_time < ?1"
        "    and app_id in (SELECT app_id FROM app);"
        ;

const char * const DatabaseSql::sqlDeleteTrafAppDay =
        "DELETE FROM traffic_app_day"
        "  WHERE traf_time < ?1"
        "    and app_id in (SELECT app_id FROM app);"
        ;

const char * const DatabaseSql::sqlDeleteTrafAppMonth =
        "DELETE FROM traffic_app_month"
        "  WHERE traf_time < ?1"
        "    and app_id in (SELECT app_id FROM app);"
        ;

const char * const DatabaseSql::sqlDeleteTrafHour =
        "DELETE FROM traffic_hour WHERE traf_time < ?1;"
        ;

const char * const DatabaseSql::sqlDeleteTrafDay =
        "DELETE FROM traffic_day WHERE traf_time < ?1;"
        ;

const char * const DatabaseSql::sqlDeleteTrafMonth =
        "DELETE FROM traffic_month WHERE traf_time < ?1;"
        ;

const char * const DatabaseSql::sqlDeleteAppTrafHour =
        "DELETE FROM traffic_app_hour"
        "  WHERE app_id = ?1;"
        ;

const char * const DatabaseSql::sqlDeleteAppTrafDay =
        "DELETE FROM traffic_app_day"
        "  WHERE app_id = ?1;"
        ;

const char * const DatabaseSql::sqlDeleteAppTrafMonth =
        "DELETE FROM traffic_app_month"
        "  WHERE app_id = ?1;"
        ;

const char * const DatabaseSql::sqlResetAppTrafTotals =
        "UPDATE app SET traf_time = ?1, in_bytes = 0, out_bytes = 0;"
        ;
