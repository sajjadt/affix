cmake_minimum_required (VERSION 2.8.11)
project (vx_host)
set(NAME vx_host)

# Options
OPTION(WITH_TESTS "Build unit tests" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_SOURCE_DIR}/3rdparty/lib)

#set "Release" build for all samples build
if ( NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "" )
    set(CMAKE_BUILD_TYPE "Release")
    message( STATUS "\nChanged from default to Release build type" )
else()
    message( STATUS "\nBuild type is ${CMAKE_BUILD_TYPE}" )
endif()

# Need to run the script provided by Intel CV SDK to export necessary enviornment variables
find_package(OpenVX REQUIRED)


set (ACOL_COMPILE_CONF_COMMAND aocl compile-config)
execute_process(COMMAND ${ACOL_COMPILE_CONF_COMMAND} OUTPUT_VARIABLE OPENCL_COMPILE_CONF)
string(REGEX REPLACE "\n$" "" OPENCL_COMPILE_CONF "${OPENCL_COMPILE_CONF}")

# OpenCL 2
# TODO: Fix
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/opt/intelFPGA_pro/18.1/hld/host/include20/ -std=c++14 -pthread -Wall -Wextra -pedantic")

message(STATUS  ${CMAKE_CXX_FLAGS})

add_executable(${NAME}
    src/cl_manager.cpp
    src/app_manager.cpp
    src/clio_manager.cpp
    src/perf.cpp
    ../common/src/AOCLUtils/opencl.cpp
    ../common/src/AOCLUtils/options.cpp
    src/main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/../common/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/include)
include_directories(${OpenVX_INCLUDE})

set (ACOL_LINK_CONF_COMMAND aocl link-config)
execute_process(COMMAND ${ACOL_LINK_CONF_COMMAND} OUTPUT_VARIABLE opencl_link)

# remove trailing white space
string(REGEX REPLACE "\n$" "" opencl_link "${opencl_link}")

# OpenCV
find_package(OpenCV PATHS ${OpenCV_DIR})
# Buidling against OpnCV provided by Intel CV SDK
if(EXISTS "${INTEL_CVSDK_DIR}/opencv/share/OpenCV" )
  set(OpenCV_DIR "${INTEL_CVSDK_DIR}/opencv/share/OpenCV")
endif()
include_directories(${OpenCV_INCLUDE})
set(LIBS ${LIBS} ${OpenCV_LIBS})

set(LIBS ${LIBS} ${OpenVX_LIBS})

target_link_libraries(${NAME} ${LIBS} ${opencl_link} ${CMAKE_DL_LIBS})

IF(WITH_TESTS)
  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIRS})
  add_executable(run_tests ${CMAKE_SOURCE_DIR}/test/tests.cpp)
  target_link_libraries(run_tests ${GTEST_LIBRARIES} pthread  ${LIBS})
ENDIF(WITH_TESTS)

