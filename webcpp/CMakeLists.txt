cmake_minimum_required(VERSION 3.10)
project(WebCppLib)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(webcpp_lib STATIC
    src/webcpp.cpp
    src/webcpp_widgets.cpp
    # Add other .c/.cpp files from WebCpp/src if any
)

target_include_directories(webcpp_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # For webcpp.h etc.
    ${CMAKE_SOURCE_DIR}/v8/include      # For V8 headers
    ${CMAKE_SOURCE_DIR}/ui/include      # For aceui_widget.h etc.
)
