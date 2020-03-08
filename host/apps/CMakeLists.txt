cmake_minimum_required (VERSION 2.8.11)

set (PROJECT "affix_plugins")
project (${PROJECT}
  LANGUAGES C CXX)
set(NAME ${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake")
message(STATUS ${CMAKE_MODULE_PATH})

#set "Release" build for all samples build
if ( NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "" )
    set(CMAKE_BUILD_TYPE "Release")
    message( STATUS "\nChanged from default to Release build type" )
else()
    message( STATUS "\nBuild type is ${CMAKE_BUILD_TYPE}" )
endif()

find_package(OpenCV PATHS ${OpenCV_DIR} REQUIRED)
set(LIBS ${LIBS} ${OpenCV_LIBS})

# CXX Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -shared -fPIC -pthread -Wall -Wextra -pedantic")

# Apps source
file(GLOB FILES
  ${CMAKE_SOURCE_DIR}/./*.cpp
)

# Compile individually
FOREACH(FILE_ ${FILES})
  MESSAGE("Found plugin: ${FILE_}")
  get_filename_component(FILE_NAME ${FILE_} NAME_WE)
  set(TARGET ${FILE_NAME})

  include_directories(${OpenCV_INCLUDE})
  include_directories(${CMAKE_SOURCE_DIR}/../include)
  include_directories(${CMAKE_SOURCE_DIR}/../../common/include)

  add_library(${TARGET} SHARED  ${FILE_})

  set_target_properties(${TARGET} PROPERTIES 
    SUBVERSION 1
    PUBLIC_HEADER application.h
  )

ENDFOREACH()
