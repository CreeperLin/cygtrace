cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -Wl,--export-dynamic")
add_executable(demo_cpp main.cc)
target_link_libraries(demo_cpp cygtrace)
