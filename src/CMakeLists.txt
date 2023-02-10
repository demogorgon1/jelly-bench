cmake_minimum_required (VERSION 3.19)

file(GLOB_RECURSE C_FILES "*.cpp")
file(GLOB_RECURSE H_FILES "*.h")

add_executable(jelly-bench ${C_FILES} ${H_FILES})
	
target_compile_features(jelly-bench PRIVATE cxx_std_20)

target_precompile_headers(jelly-bench PRIVATE "Pcheader.h")
	
target_link_libraries(jelly-bench jelly::jelly)

install(TARGETS jelly-bench RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
