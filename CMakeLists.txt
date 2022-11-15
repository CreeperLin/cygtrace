cmake_minimum_required(VERSION 3.9)
project(cygtrace
    DESCRIPTION "cygtrace"
)
file(READ "VERSION" CMAKE_PROJECT_VERSION)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -finstrument-functions -Wl,--export-dynamic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -Wl,--export-dynamic")
include(GNUInstallDirs)
add_library(cygtrace SHARED
    cygtrace.cpp
)
if(CYGTRACE_DEMANGLE)
    target_compile_definitions(cygtrace PRIVATE CYGTRACE_DEMANGLE=1)
endif()
set_target_properties(cygtrace PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "cygtrace_export.h;cygtrace.h"
)
target_link_libraries(cygtrace dl)
configure_file(cygtrace.pc.in cygtrace.pc @ONLY)
target_include_directories(cygtrace PRIVATE .)
install(TARGETS cygtrace
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/cygtrace.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

if(CYGTRACE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(CYGTRACE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
