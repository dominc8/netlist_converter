cmake_minimum_required(VERSION 3.14)

set(ENV{CMAKE_EXPORT_COMPILE_COMMANDS} TRUE)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Release as default")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type" FORCE)
endif()

project (netlist_converter)

include(FetchContent)
FetchContent_Declare(
    ogdf
    GIT_REPOSITORY https://github.com/ogdf/ogdf.git
    GIT_TAG catalpa-202002
)
FetchContent_MakeAvailable(ogdf)

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_subdirectory(src)

add_library(${PROJECT_NAME} ${SRC_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

add_executable(app ${CMAKE_SOURCE_DIR}/src/app.cpp)
target_link_libraries(app ${PROJECT_NAME} OGDF)

#############################################
# Unit tests

add_subdirectory(ut)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Add a testing executable
add_executable(unit_tests ${UT_SRC_FILES})

target_link_libraries(unit_tests
    ${PROJECT_NAME}
    gtest_main
)

add_custom_target(run_uts GTEST_COLOR=1 ${CMAKE_BINARY_DIR}/unit_tests DEPENDS unit_tests)

