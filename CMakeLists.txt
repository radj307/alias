# cmd-alias
cmake_minimum_required (VERSION 3.15)

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)			# Export all symbols on windows
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")	# Fix MSVC __cplusplus macro
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
include(AutoVersion)

# Create the version.h file
FILE(REMOVE "${CMAKE_SOURCE_DIR}/alias/version.h")
PARSE_VERSION_STRING(${CMAKE_SOURCE_DIR} VER_MAJOR VER_MINOR VER_PATCH VER_EXTRA)
CONCAT_VERSION_STRING(VER ${VER_MAJOR} ${VER_MINOR} ${VER_PATCH})
configure_file("${CMAKE_SOURCE_DIR}/cmake/inputs/version.h.in" "${CMAKE_SOURCE_DIR}/alias/version.h" @ONLY)

# Command Alias Project
project ("cmd-alias" VERSION ${VER} LANGUAGES CXX)

# Include the main alias project.
add_subdirectory("alias")

