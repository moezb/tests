cmake_minimum_required(VERSION 3.18)
find_library(apr-1  PATHS  ENV APR_ROOT)
add_executable(TestJsonPack main.c common.c input_data.c dictionary_data.c output_data.c) 
target_link_libraries(TestJsonPack  apr-1)

