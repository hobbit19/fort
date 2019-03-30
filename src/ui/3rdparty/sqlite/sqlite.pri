CONFIG(release, debug|release): DEFINES *= NDEBUG

DEFINES += \
    SQLITE_THREADSAFE=1 \
    SQLITE_DEFAULT_LOCKING_MODE=1 \
    SQLITE_DEFAULT_MEMSTATUS=0 \
    SQLITE_DEFAULT_WAL_SYNCHRONOUS=1 \
    SQLITE_LIKE_DOESNT_MATCH_BLOBS \
    SQLITE_USE_ALLOCA \
    SQLITE_OMIT_AUTHORIZATION \
    SQLITE_OMIT_AUTOINIT \
    SQLITE_OMIT_AUTOMATIC_INDEX \
    SQLITE_OMIT_BLOB_LITERAL \
    SQLITE_OMIT_CAST \
    SQLITE_OMIT_CHECK \
    SQLITE_OMIT_COMPLETE \
    SQLITE_OMIT_DECLTYPE \
    SQLITE_OMIT_DEPRECATED \
    SQLITE_OMIT_EXPLAIN \
    SQLITE_OMIT_FOREIGN_KEY \
    SQLITE_OMIT_GET_TABLE \
    SQLITE_OMIT_INCRBLOB \
    SQLITE_OMIT_INTEGRITY_CHECK \
    SQLITE_OMIT_LOAD_EXTENSION \
    SQLITE_OMIT_PROGRESS_CALLBACK \
    SQLITE_OMIT_SHARED_CACHE \
    SQLITE_OMIT_TCL_VARIABLE \
    SQLITE_OMIT_TEMPDB \
    SQLITE_OMIT_TRACE

SQLITE_DIR = $$PWD/../../../3rdparty/sqlite
INCLUDEPATH += $$SQLITE_DIR

SOURCES += \
    $$SQLITE_DIR/sqlite3.c \
    $$PWD/sqlitedb.cpp \
    $$PWD/sqliteengine.cpp \
    $$PWD/sqlitestmt.cpp

HEADERS += \
    $$SQLITE_DIR/sqlite3.h \
    $$PWD/sqlitedb.h \
    $$PWD/sqliteengine.h \
    $$PWD/sqlitestmt.h
