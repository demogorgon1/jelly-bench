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

include(FetchContent)
include(GNUInstallDirs)

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

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(
		-Wall 
		-Wextra 
		-pedantic 
		-Werror 
		-Wno-unused-function 	# I want unused functions for debugging purposes
		-Wno-reorder			# I want to be able to initialize class members in any order
		-Wno-class-memaccess	# I want to be able to memset() things that I know are safe to memset()
	)	
endif()

add_subdirectory(src)