# Photos Application (WebCpp-based)
add_executable(PhotosApp Main.cpp)

# Add include directory for WebCpp
target_include_directories(PhotosApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../WebCpp/include
)

# Link with WebCpp library
target_link_libraries(PhotosApp PRIVATE webcpp_lib pthread)

target_compile_features(PhotosApp PRIVATE cxx_std_17)

# Copy HTML file to the build directory next to the executable
add_custom_command(
    TARGET PhotosApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/Photos.html
            $<TARGET_FILE_DIR:PhotosApp>/Photos.html
    COMMENT "Copying Photos.html for PhotosApp"
)