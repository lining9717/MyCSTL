cmake_minimum_required (VERSION 2.8)

project(MyCSTL)

include_directories(include)
aux_source_directory(. ALL_SRCS)

add_subdirectory(src)

add_executable(main ${ALL_SRCS})

target_link_libraries(main cstllib)
