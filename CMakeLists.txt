cmake_minimum_required(VERSION 3.10)
project(Chess)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# SFML path
set(SFML_ROOT "${CMAKE_SOURCE_DIR}/lib/SFML")
set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
set(SFML_LIB_DIR "${SFML_ROOT}/lib")

# collect source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/*.hpp")

# add executable
add_executable(Chess ${SRC_FILES} ${HEADER_FILES})

# ensure CMake re-runs if a new file is added
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# include SFML headers
target_include_directories(Chess PRIVATE ${SFML_INCLUDE_DIR} ${SOURCE_DIR})

# link SFML libraries
target_link_directories(Chess PRIVATE ${SFML_LIB_DIR})

# debug postfix for Windows
if(WIN32)
    set(SFML_DEBUG_POSTFIX "-d")
endif()

# link appropriate libraries based on build type
target_link_libraries(Chess PRIVATE
    debug sfml-system${SFML_DEBUG_POSTFIX}
    debug sfml-window${SFML_DEBUG_POSTFIX}
    debug sfml-graphics${SFML_DEBUG_POSTFIX}
    debug sfml-audio${SFML_DEBUG_POSTFIX}
    optimized sfml-system
    optimized sfml-window
    optimized sfml-graphics
    optimized sfml-audio
)

# copy SFML DLLs into build directory (Windows only)
if(WIN32 AND EXISTS "${SFML_ROOT}/bin")
    add_custom_command(TARGET Chess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_ROOT}/bin" $<TARGET_FILE_DIR:Chess>)
endif()

# copy assets into build directory
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
if(EXISTS ${ASSETS_DIR})
    add_custom_command(TARGET Chess POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSETS_DIR} $<TARGET_FILE_DIR:Chess>/assets)
    message(STATUS "Assets directory will be copied to build directory")
else()
    message(WARNING "Assets directory not found at: ${ASSETS_DIR}")
endif()

# ensure the logs directory is created in the executable's directory
add_custom_command(TARGET Chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:Chess>/logs
)

# debug output to verify SFML paths
message(STATUS "SFML_ROOT: ${SFML_ROOT}")
message(STATUS "SFML_INCLUDE_DIR: ${SFML_INCLUDE_DIR}")
message(STATUS "SFML_LIB_DIR: ${SFML_LIB_DIR}")

# debug output to verify collected source files
message(STATUS "Source files: ${SRC_FILES}")
message(STATUS "Header files: ${HEADER_FILES}")
