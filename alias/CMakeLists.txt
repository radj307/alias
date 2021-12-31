# cmd-alias/alias
cmake_minimum_required (VERSION 3.15)

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(INCLUDE_FILES
	"globals.h"
	"config.hpp"
	"main.cpp"
)

add_executable (alias ${INCLUDE_FILES})
#target_include_directories(alias PUBLIC "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/307lib/${CMAKE_BUILD_TYPE}/include")
#target_link_directories(alias PUBLIC "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/307lib/${CMAKE_BUILD_TYPE}/lib")

target_link_libraries(alias PUBLIC shared strlib filelib TermAPI)

install(
	TARGETS alias
	RUNTIME
	DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)
