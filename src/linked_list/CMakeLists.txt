# Add the executable
add_executable(linked_list main.cpp)

# Link the library to the executable
target_link_libraries(linked_list PRIVATE pointers_library)
