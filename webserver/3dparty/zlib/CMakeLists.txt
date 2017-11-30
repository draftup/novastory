project (zlib)
cmake_minimum_required (VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
aux_source_directory(${PROJECT_SOURCE_DIR} zlib_srcs)
add_library (zlib STATIC ${zlib_srcs})
