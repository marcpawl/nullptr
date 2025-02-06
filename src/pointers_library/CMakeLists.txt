include(GenerateExportHeader)


add_library(pointers_library pointers_library.cpp)



add_library(myproject::pointers_library ALIAS pointers_library)

target_link_libraries(pointers_library PRIVATE myproject_options myproject_warnings)

target_include_directories(pointers_library ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(pointers_library PUBLIC cxx_std_20)

set_target_properties(
  pointers_library
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(pointers_library EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/myproject/pointers_library_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(pointers_library PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()
