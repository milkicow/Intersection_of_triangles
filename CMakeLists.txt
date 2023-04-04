cmake_minimum_required(VERSION 3.2)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(INTERSECTION_OF_TRIANGLES LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17  " )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(SOURCE ${PROJECT_SOURCE_DIR}/main.cpp)
aux_source_directory(./src SOURCES)

add_executable(${PROJECT_NAME} ${SOURCE} ${SOURCES})

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_program(glslc NAMES glslc HINTS Vulkan::glslc REQUIRED)



function(add_spirv_shader TARGET_NAME INPUT_FILE)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.spv
    COMMAND ${glslc} ${INPUT_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.spv
    MAIN_DEPENDENCY ${INPUT_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  add_custom_target(${TARGET_NAME} ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.spv)

  set(${TARGET_NAME}_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.spv PARENT_SCOPE)
endfunction()


add_spirv_shader(vert shaders/shader.vert)
add_spirv_shader(frag shaders/shader.frag)
add_custom_target(shaders ALL DEPENDS vert frag)

include_directories(${Vulkan_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES}  Vulkan::Vulkan glfw)

target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT ${PROJECT_NAME})

add_subdirectory(test)

