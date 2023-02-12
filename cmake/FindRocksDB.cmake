find_path(ROCKSDB_INCLUDE_DIR NAMES rocksdb/db.h)

find_library(ROCKSDB_LIBRARY_DEBUG NAMES rocksdb rocksdb-shared)
find_library(ROCKSDB_LIBRARY_RELEASE NAMES rocksdb rocksdb-shared)

include(SelectLibraryConfigurations)
SELECT_LIBRARY_CONFIGURATIONS(ROCKSDB)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    RocksDB DEFAULT_MSG
    ROCKSDB_LIBRARY ROCKSDB_INCLUDE_DIR
)

if (ROCKSDB_FOUND)
    message(STATUS "Found RocksDB: ${ROCKSDB_LIBRARY}")
endif()
