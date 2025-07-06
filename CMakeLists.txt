cmake_minimum_required(VERSION 3.16)
project(lambdaOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories for kernel, ui, webcpp, etc.
add_subdirectory(kernel)
add_subdirectory(ui)
add_subdirectory(webcpp)
add_subdirectory(apps)

# Optionally add tools if needed
# add_subdirectory(tools)
