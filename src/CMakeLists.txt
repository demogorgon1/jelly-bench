cmake_minimum_required (VERSION 3.19)

file(GLOB_RECURSE C_FILES "*.cpp")
file(GLOB_RECURSE H_FILES "*.h")

add_executable(jelly-bench ${C_FILES} ${H_FILES})

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
	
target_compile_features(jelly-bench PRIVATE cxx_std_20)
target_include_directories(jelly-bench PRIVATE ${ROCKSDB_INCLUDE_DIR})
target_precompile_headers(jelly-bench PRIVATE "Pcheader.h")
	
if(MSVC)
	# needed by rocksdb
	set(EXTRA_LIBRARIES rpcrt4 shlwapi)
else()
	set(EXTRA_LIBRARIES pthread dl)
endif()
	
target_link_libraries(jelly-bench jelly::jelly sqlite::sqlite ${ROCKSDB_LIBRARY} ${EXTRA_LIBRARIES})

install(TARGETS jelly-bench RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
