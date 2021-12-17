# cmd-alias
cmake_minimum_required (VERSION 3.15)

# require C++20
set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Fix MSVC __cplusplus macro
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

project ("cmd-alias")

set(TERMAPI_VERSION 4)
set(TERMAPI_ENABLE_EXTENSIONS ON)
add_subdirectory("307lib")
add_subdirectory("alias")
