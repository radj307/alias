# alias/execpipe
cmake_minimum_required(VERSION 3.20)

add_executable(execpipe "main.cpp")

set_property(TARGET execpipe PROPERTY CXX_STANDARD 20)
set_property(TARGET execpipe PROPERTY CXX_STANDARD_REQUIRED ON)

target_compile_options(execpipe PRIVATE "/Zc:__cplusplus")

target_link_libraries(execpipe PUBLIC shared TermAPI)
