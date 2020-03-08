cmake_minimum_required (VERSION 2.8.11)
project (host)
set(PROJECT_NAME host)
set(CMAKE_CXX_STANDARD 11)

# Options
OPTION(WITH_TESTS "Build unit tests" OFF)


#set "Release" build for all samples build
if ( NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "" )
    set(CMAKE_BUILD_TYPE "Release")
    message( STATUS "\nChanged from default to Release build type" )
else()
    message( STATUS "\nBuild type is ${CMAKE_BUILD_TYPE}" )
endif()

set (ACOL_COMPILE_CONF_COMMAND aocl compile-config)
execute_process(COMMAND ${ACOL_COMPILE_CONF_COMMAND} OUTPUT_VARIABLE OPENCL_COMPILE_CONF)
string(REGEX REPLACE "\n$" "" OPENCL_COMPILE_CONF "${OPENCL_COMPILE_CONF}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENCL_COMPILE_CONF} -std=c++14 -pthread -Wall -Wextra -pedantic")

add_executable(${PROJECT_NAME}
    src/opencl_manager.cpp
    src/application_manager.cpp
    src/opencl_io_manager.cpp
    ../common/src/AOCLUtils/opencl.cpp
    ../common/src/AOCLUtils/options.cpp
    src/main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/../common/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/include)

set (ACOL_LINK_CONF_COMMAND aocl link-config)
execute_process(COMMAND ${ACOL_LINK_CONF_COMMAND} OUTPUT_VARIABLE opencl_link)
# Remove trailing white space
string(REGEX REPLACE "\n$" "" opencl_link "${opencl_link}")

find_package(OpenCV PATHS ${OpenCV_DIR})
include_directories(${OpenCV_INCLUDE})
# find_package(glog REQUIRED CONFIG NAMES google-glog glog)
set(LIBS ${LIBS} ${OpenCV_LIBS})
#  ${GLOG_LIBRARIES})

target_link_libraries(${PROJECT_NAME} ${LIBS} ${opencl_link} ${CMAKE_DL_LIBS})

IF(WITH_TESTS)
  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIRS})
  add_executable(run_tests ${CMAKE_SOURCE_DIR}/test/tests.cpp)
  target_link_libraries(run_tests ${GTEST_LIBRARIES} pthread  ${LIBS})
ENDIF(WITH_TESTS)
