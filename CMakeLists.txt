# cmd-alias
cmake_minimum_required (VERSION 3.15)

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)			# Export all symbols on windows
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")	# Fix MSVC __cplusplus macro
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/307modules")
include(AutoVersion)

# Create the version.h file
FILE(REMOVE "${CMAKE_SOURCE_DIR}/alias/version.h")
GET_VERSION(${CMAKE_SOURCE_DIR} VER_MAJOR VER_MINOR VER_PATCH VER_EXTRA)
MAKE_VERSION(VER ${VER_MAJOR} ${VER_MINOR} ${VER_PATCH})

# Command Alias Project
project ("cmd-alias" VERSION ${VER} LANGUAGES CXX)

add_subdirectory("307lib")

# Include the main alias project.
add_subdirectory("alias")

option(ENABLE_EXECPIPE "" OFF)
# Include the execute contents of STDOUT as command project
if (ENABLE_EXECPIPE)
	add_subdirectory("execpipe")
endif()
