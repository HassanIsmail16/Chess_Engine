cmake_minimum_required(VERSION 3.10)

# project setup
project(Chess)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add SFML paths
set(SFML_ROOT "${CMAKE_SOURCE_DIR}/libs/SFML")
set(CMAKE_MODULE_PATH "${SFML_ROOT}/cmake/Modules" ${CMAKE_MODULE_PATH})

# add executable
file(GLOB SRC_FILES "src/*.cpp")
file(GLOB HEADER_FILES "src/*.h")

add_executable(Chess ${SRC_FILES} ${HEADER_FILES})

# link SFML
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)
target_include_directories(Chess PRIVATE ${SFML_ROOT}/include)
target_link_directories(Chess PRIVATE ${SFML_ROOT}/lib)

# link libraries
target_link_libraries(Chess PRIVATE sfml-system sfml-window sfml-graphics)
