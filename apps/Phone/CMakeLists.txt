# Phone Application (WebCpp-based)
add_executable(PhoneApp Main.cpp)

# Add include directory for WebCpp
target_include_directories(PhoneApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../WebCpp/include
)

# Link with WebCpp library
target_link_libraries(PhoneApp PRIVATE webcpp_lib pthread)

target_compile_features(PhoneApp PRIVATE cxx_std_17)

# Copy HTML file to the build directory next to the executable
# Assumes Main.cpp loads "Phone.html"
add_custom_command(
    TARGET PhoneApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/Phone.html # Ensure this HTML file exists for WebCpp
            $<TARGET_FILE_DIR:PhoneApp>/Phone.html
    COMMENT "Copying Phone.html for PhoneApp"
)