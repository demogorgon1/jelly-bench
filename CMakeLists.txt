cmake_minimum_required (VERSION 3.19)

project(jelly-bench)

macro(jelly_bench_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow overriding the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

jelly_bench_option(JELLY_BENCH_ROCKSDB "Enable RocksDB." OFF)

include(FetchContent)
include(GNUInstallDirs)

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  ${CMAKE_MODULE_PATH})

# --------------------------------------------------------------------

if (JELLY_BENCH_ROCKSDB)
	find_package(RocksDB)
endif()

# --------------------------------------------------------------------

set(JELLY_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(JELLY_TEST OFF CACHE INTERNAL "" FORCE)
set(JELLY_ZSTD ON CACHE INTERNAL "" FORCE)

add_definitions(-DJELLY_ZSTD)

FetchContent_Declare(jelly
  GIT_REPOSITORY https://github.com/demogorgon1/jelly.git
)
FetchContent_MakeAvailable(jelly)

# --------------------------------------------------------------------

include_directories(sqlite)

add_subdirectory(sqlite)
add_subdirectory(src)

