cmake_minimum_required(VERSION 3.2) 

project(INTERSECRION_OF_TRIANGLES LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCE ${PROJECT_SOURCE_DIR}/main.cpp)
aux_source_directory(./src SRC_LIST)
add_executable(intersect ${SOURCE} ${SRC_LIST})

target_include_directories(intersect
                            PRIVATE include)

install(TARGETS intersect
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT intersect)

add_subdirectory(test)
