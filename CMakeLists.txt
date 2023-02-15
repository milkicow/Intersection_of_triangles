cmake_minimum_required(VERSION 3.2) 

project(INTERSECRION_OF_TRIANGLES LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCE ${PROJECT_SOURCE_DIR}/src/line.cpp
           ${PROJECT_SOURCE_DIR}/src/plane.cpp
           ${PROJECT_SOURCE_DIR}/src/triangle.cpp
           ${PROJECT_SOURCE_DIR}/src/vector.cpp
           ${PROJECT_SOURCE_DIR}/src/intersect.cpp
           ${PROJECT_SOURCE_DIR}/src/segment.cpp
           ${PROJECT_SOURCE_DIR}/main.cpp         )
        
add_executable(intersect ${SOURCE})
target_include_directories(intersect
                            PRIVATE include)

install(TARGETS intersect
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT intersect)
