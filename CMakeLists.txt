cmake_minimum_required( VERSION 3.28 FATAL_ERROR )

project( partitioned_future
    VERSION 0.1.0
    DESCRIPTION ""
    LANGUAGES CXX
)

set( HEADERS
    include/partitioned_future/algorithm.h
    include/partitioned_future/partitioned_future.h
    include/partitioned_future/std_execution.h
)

add_library( ${PROJECT_NAME} INTERFACE
    ${HEADERS}
)

target_compile_features( ${PROJECT_NAME} 
    INTERFACE 
        cxx_std_17
)

target_include_directories( ${PROJECT_NAME} 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_subdirectory( tests )