# Define the library
add_library(pointers_library STATIC pointers_library.cpp include/pointers.hpp)

# Specify the include directories for the library
target_include_directories(pointers_library PUBLIC include)
