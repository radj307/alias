# alias/
cmake_minimum_required (VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/307lib/307modules")
include(VersionTag)

set(ENV{alias_VERSION} "0.0.0")
GET_VERSION_TAG("${CMAKE_CURRENT_SOURCE_DIR}" "alias")


add_subdirectory("307lib")
add_subdirectory("alias")


option(ENABLE_EXECPIPE "" OFF)
# Include the execute contents of STDOUT as command project
if (ENABLE_EXECPIPE)
	add_subdirectory("execpipe")
endif()
