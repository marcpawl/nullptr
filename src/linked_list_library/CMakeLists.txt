# Define the library
add_library(linked_list_library STATIC include/doublelist.hpp)
set_target_properties(linked_list_library PROPERTIES LINKER_LANGUAGE CXX)

# Specify the include directories for the library
target_include_directories(linked_list_library PUBLIC include)
