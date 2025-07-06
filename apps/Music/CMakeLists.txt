# Music Application (WebCpp-based)
add_executable(MusicApp Main.cpp)

# Add include directory for WebCpp
target_include_directories(MusicApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../WebCpp/include
)

# Link with WebCpp library
target_link_libraries(MusicApp PRIVATE webcpp_lib pthread) # Add audio libs if needed

target_compile_features(MusicApp PRIVATE cxx_std_17)

# Copy HTML file to the build directory next to the executable
add_custom_command(
    TARGET MusicApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/Music.html
            $<TARGET_FILE_DIR:MusicApp>/Music.html
    COMMENT "Copying Music.html for MusicApp"
)