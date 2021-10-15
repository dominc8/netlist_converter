cmake_minimum_required(VERSION 3.5)

set(ENV{CMAKE_EXPORT_COMPILE_COMMANDS} TRUE)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Release as default")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type" FORCE)
endif()

project (net_asc_converter)


include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)

add_library(${PROJECT_NAME} ${SRC_FILES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

#############################################
# Unit tests

#add_subdirectory(3rd_party/google-test)
add_subdirectory(ut)

find_package(GTest REQUIRED)

# Add a testing executable
add_executable(unit_tests ${UT_SRC_FILES})

target_link_libraries(unit_tests
    ${PROJECT_NAME}
    GTest::GTest 
    GTest::Main
)

add_custom_target(run_uts GTEST_COLOR=1 ${CMAKE_BINARY_DIR}/unit_tests DEPENDS unit_tests)
