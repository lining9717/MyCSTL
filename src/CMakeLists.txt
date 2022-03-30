include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(. LIB_SRC)
add_library(cstllib ${LIB_SRC})

