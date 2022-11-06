cmake_minimum_required(VERSION 3.8)
find_package(pybind11 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/../)
include_directories(${pybind11_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -Wl,--export-dynamic")

add_library(demo_pybind11 MODULE demo.cc)
target_link_libraries(demo_pybind11 PUBLIC pybind11::module)
# target_link_libraries(demo_pybind11 PUBLIC cygtrace)
# target_compile_features(demo_pybind11 PUBLIC cxx_std_14)
set_target_properties(demo_pybind11 PROPERTIES OUTPUT_NAME "demo")

configure_file(test.py test.py COPYONLY)
configure_file(run.sh run.sh COPYONLY)
