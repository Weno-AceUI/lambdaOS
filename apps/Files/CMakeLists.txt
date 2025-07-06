# Files Application (WebCpp-based)
add_executable(FilesApp Main.cpp)

# Add include directory for WebCpp
target_include_directories(FilesApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../WebCpp/include
)

# Link with WebCpp library
target_link_libraries(FilesApp PRIVATE webcpp_lib pthread)

target_compile_features(FilesApp PRIVATE cxx_std_17) 

# Copy HTML file to the build directory next to the executable
# Assumes Main.cpp loads "Files.html"
add_custom_command(
    TARGET FilesApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/Files.html # Ensure this HTML file exists for WebCpp
            $<TARGET_FILE_DIR:FilesApp>/Files.html
    COMMENT "Copying Files.html for FilesApp"
)