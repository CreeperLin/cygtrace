find_package(Python3 COMPONENTS Development)

include_directories(${Python3_INCLUDE_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finstrument-functions -Wl,--export-dynamic")

add_library(demo_cpython MODULE demo.c)
target_link_libraries(demo_cpython PUBLIC ${Python3_LIBRARIES})
set_target_properties(demo_cpython PROPERTIES OUTPUT_NAME "demo")

configure_file(test.py test.py COPYONLY)
configure_file(run.sh run.sh COPYONLY)
