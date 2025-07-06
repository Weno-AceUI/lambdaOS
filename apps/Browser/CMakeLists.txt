# Browser Application (WebCpp-based)
add_executable(BrowserApp Main.cpp)

# Add include directory for WebCpp
target_include_directories(BrowserApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../WebCpp/include
)

# Link with WebCpp library
target_link_libraries(BrowserApp PRIVATE webcpp_lib pthread) # Add other deps like curl if webcpp_load_url uses it

target_compile_features(BrowserApp PRIVATE cxx_std_17)

# Copy HTML file to the build directory next to the executable
add_custom_command(
    TARGET BrowserApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/Browser.html
            $<TARGET_FILE_DIR:BrowserApp>/Browser.html
    COMMENT "Copying Browser.html for BrowserApp"
)