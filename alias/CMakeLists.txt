# alias/alias
cmake_minimum_required (VERSION 3.20)

file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"*.h*"
)
file(GLOB SRCS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"*.c*"
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/rc")
MAKE_VERSION_HEADER("${CMAKE_CURRENT_BINARY_DIR}/rc/version.h" "alias" "${alias_VERSION_EXTENDED}")

include(ResourceMaker)
MAKE_STRINGRC_VERSIONINFO(_alias_stringrc_versioninfo
	"${alias_VERSION_EXTENDED}"
	"Copyright © 2023 by radj307"
	"radj307"
	"alias"
	"Simple command alias utility."
)
MAKE_RESOURCE("${CMAKE_CURRENT_BINARY_DIR}/rc/alias.rc" "${_alias_stringrc_versioninfo}")

add_executable(alias "${SRCS}" "${CMAKE_CURRENT_BINARY_DIR}/rc/version.h" "${CMAKE_CURRENT_BINARY_DIR}/rc/alias.rc")

set_property(TARGET alias PROPERTY CXX_STANDARD 20)
set_property(TARGET alias PROPERTY CXX_STANDARD_REQUIRED ON)

target_sources(alias PUBLIC "${HEADERS}")

target_include_directories(alias PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/rc")

target_compile_options(alias PRIVATE "/Zc:__cplusplus")

target_link_libraries(alias PUBLIC shared strlib filelib TermAPI xlog)
