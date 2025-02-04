cmake_minimum_required(VERSION 3.10)
project(Chess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# SFML path
set(SFML_ROOT "${CMAKE_SOURCE_DIR}/lib/SFML")
set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
set(SFML_LIB_DIR "${SFML_ROOT}/lib")

# add executable
file(GLOB SRC_FILES "src/*.cpp")
file(GLOB HEADER_FILES "src/*.h")
add_executable(Chess ${SRC_FILES} ${HEADER_FILES})

# include SFML headers
target_include_directories(Chess PRIVATE ${SFML_INCLUDE_DIR})

# link SFML libraries
target_link_directories(Chess PRIVATE ${SFML_LIB_DIR})
target_link_libraries(Chess PRIVATE sfml-system sfml-window sfml-graphics)

# copy SFML DLLs into build directory (windows only)
if(WIN32 AND EXISTS "${SFML_ROOT}/bin")
    add_custom_command(TARGET Chess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_ROOT}/bin" $<TARGET_FILE_DIR:Chess>)
endif()
