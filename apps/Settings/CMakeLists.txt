# Settings Application (WebCpp-based)
add_executable(SettingsApp Main.cpp)

# Add include directory for WebCpp
target_include_directories(SettingsApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../WebCpp/include
)

# Link with WebCpp library
target_link_libraries(SettingsApp PRIVATE webcpp_lib pthread)

target_compile_features(SettingsApp PRIVATE cxx_std_17) 

# Copy HTML file to the build directory next to the executable
# Assumes Main.cpp loads "Settings.html"
add_custom_command(
    TARGET SettingsApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/Settings.html # This HTML file is already named correctly
            $<TARGET_FILE_DIR:SettingsApp>/Settings.html
    COMMENT "Copying Settings.html for SettingsApp"
)