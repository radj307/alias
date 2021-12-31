cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
if (msvc)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus")
endif()

set(INCLUDE_FILES
	"main.cpp"
)

add_executable(execpipe ${INCLUDE_FILES})
target_include_directories(execpipe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/307lib/${CMAKE_BUILD_TYPE}/include")
target_link_directories(execpipe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/307lib/${CMAKE_BUILD_TYPE}/lib")

target_link_libraries(execpipe PUBLIC shared TermAPI)

install(
	TARGETS execpipe
	RUNTIME
	DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)
