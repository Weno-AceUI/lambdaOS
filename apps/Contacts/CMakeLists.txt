# Contacts Application (WebCpp-based)
add_executable(ContactsApp Main.cpp)

# Add include directory for WebCpp
# Assumes WebCpp/include is two levels up from current source dir
target_include_directories(ContactsApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../WebCpp/include
)

# Link with WebCpp library (assuming it's built as webcpp_lib)
target_link_libraries(ContactsApp PRIVATE webcpp_lib pthread)

# Main.cpp is C, so ensure C standard is set if needed, or CXX if it's C++
target_compile_features(ContactsApp PRIVATE c_std_11) # Or cxx_std_17 if Main.cpp is C++

# Copy HTML file to the build directory next to the executable
# Assumes Main.cpp loads "Contacts.html"
add_custom_command(
    TARGET ContactsApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/Contacts.html  # Ensure this HTML file exists for WebCpp
            $<TARGET_FILE_DIR:ContactsApp>/Contacts.html
    COMMENT "Copying Contacts.html for ContactsApp"
)