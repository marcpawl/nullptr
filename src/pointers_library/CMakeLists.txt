# Define the library
add_library(pointers_library STATIC pointers_library.cpp include/pointers.hpp)
set_target_properties(pointers_library PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pointers_library PUBLIC GSL)

# Specify the include directories for the library
target_include_directories(pointers_library PUBLIC include)
