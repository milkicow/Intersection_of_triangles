find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB CPP_SOURCES ../src/*.cpp)
add_executable(tests ${CPP_SOURCES} gtest.cpp)
target_include_directories(tests PRIVATE ../include)
 
target_link_libraries(tests PRIVATE ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)

add_definitions(-DPROJECT_DIR_PATH=\"${CMAKE_SOURCE_DIR}\")

install(TARGETS tests
RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
COMPONENT tests)