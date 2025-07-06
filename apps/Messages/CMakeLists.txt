# Messages Application (WebCpp-based)
add_executable(MessagesApp Main.cpp)

# Add include directory for WebCpp
target_include_directories(MessagesApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../WebCpp/include
)

# Link with WebCpp library
target_link_libraries(MessagesApp PRIVATE webcpp_lib pthread)

target_compile_features(MessagesApp PRIVATE cxx_std_17) 

# Copy HTML file to the build directory next to the executable
# Assumes Main.cpp loads "Messages.html"
add_custom_command(
    TARGET MessagesApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/Messages.html # Ensure this HTML file exists for WebCpp
            $<TARGET_FILE_DIR:MessagesApp>/Messages.html
    COMMENT "Copying Messages.html for MessagesApp"
)