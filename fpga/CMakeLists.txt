cmake_minimum_required (VERSION 2.6)
project (cl_compile)

# Makefile generator for
# 1) Compile tests/apps for emulation
# 2) Compile tests/apps for RTL evaluation
# TODO: 3) Compile tests/apps for FPGA (two passes)

# In order to pass extra compile flags use AOCFLAGS
# e.g.,   make x-emul AOCFLAGS=-DVERBOSE

set(NAME cl_tests)
set(BOARD a10gx_hostch)

# Options
option(BSP_DIR "Borad support package directory" "")

# Headers
set(INC ${CMAKE_SOURCE_DIR}/include)

file(GLOB DEPS
 ${CMAKE_SOURCE_DIR}/include/*.h
 ${CMAKE_SOURCE_DIR}/include/templates/*.h
 ${CMAKE_SOURCE_DIR}/include/kernels/*.h
)

file(GLOB FILES
  ${CMAKE_SOURCE_DIR}/sample/*.cl
)

set(FLAGS "")

IF(NOT BSP_DIR STREQUAL "OFF")
  set(FLAGS ${FLAGS} "-board-package=${BSP_DIR}")
  message(STATUS "Using board package in: " ${BSP_DIR})
ELSE()
  message(STATUS "Using default board package")
ENDIF()

FOREACH(FILE_ ${FILES})
  MESSAGE("Including pipeline from: ${FILE_}")
  get_filename_component(FILE_NAME ${FILE_} NAME_WE)
  set(TARGET ${FILE_NAME}-fpga.aocx)

  add_custom_target (
    ${FILE_NAME}-emul
    COMMAND aoc $(AOCFLAGS) ${FLAGS} ${FILE_} -I${INC} -march=emulator -DEMULATION -o ${FILE_NAME}-emul
    DEPENDS ${DEPS}
    COMMENT " Compiling for emulation"
  )

  add_custom_target (
    ${FILE_NAME}-rtl
    COMMAND aoc $(AOCFLAGS) ${FLAGS} ${FILE_} -I${INC} -board=${BOARD} -rtl -v -o ${FILE_NAME}-rtl
    DEPENDS ${DEPS}
    COMMENT " Compiling to RTL"
  )

  # Set profile=all by default
  add_custom_target (
    ${FILE_NAME}-fpga
    COMMAND aoc $(AOCFLAGS) ${FLAGS} ${FILE_} -I${INC} -board=${BOARD} -profile=all -v -o ${FILE_NAME}-fpga
    DEPENDS ${DEPS}
    COMMENT " Compiling to FPGA"
  )
ENDFOREACH()
