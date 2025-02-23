# Define the library

# Setup include directory
add_subdirectory(include)

add_library(pointers_library STATIC include)
set_target_properties(pointers_library PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pointers_library PUBLIC GSL)

# Specify the include directories for the library
target_include_directories(pointers_library PUBLIC include)

#target_clangformat_setup(pointers_library)
clangformat_setup( include/marcpawl/pointers/* )
