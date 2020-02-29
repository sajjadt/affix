unset(ITT_INCLUDE_DIR CACHE)
unset(ITT_LIB CACHE)

if(NOT DEFINED INTEL_VTUNE_DIR AND DEFINED ENV{INTEL_VTUNE_DIR})
    set(INTEL_VTUNE_DIR "$ENV{INTEL_VTUNE_DIR}")
endif()
if(NOT DEFINED INTEL_VTUNE_DIR AND DEFINED ENV{VTUNE_AMPLIFIER_XE_2016_DIR})
    set(INTEL_VTUNE_DIR "$ENV{VTUNE_AMPLIFIER_XE_2016_DIR}")
endif()
if(NOT DEFINED INTEL_VTUNE_DIR AND EXISTS "/opt/intel/vtune_amplifier_xe/include")
    set(INTEL_VTUNE_DIR "/opt/intel/vtune_amplifier_xe")
endif()

if(DEFINED INTEL_VTUNE_DIR)
    message(STATUS "INTEL_VTUNE_DIR = ${INTEL_VTUNE_DIR}")

    util_find_dir(ITT_INCLUDE_DIR
    FILES
        ittnotify.h
    PATHS "${INTEL_VTUNE_DIR}/include/"
    DOC "Intel OpenVX include directory")

    find_library(ITT_LIB
    "${CMAKE_STATIC_LIBRARY_PREFIX}ittnotify${CMAKE_STATIC_LIBRARY_SUFFIX}"
    PATHS ${INTEL_VTUNE_DIR}/lib64)

    set(Located_ITT_LIBS ${ITT_LIB} ${CMAKE_DL_LIBS})
    set(Located_ITT_INCLUDE_DIRS ${ITT_INCLUDE_DIR})
else()
    message(STATUS "INTEL_VTUNE_DIR is not defined")
endif()

# Handle find_package() arguments, and set INTEL_ITT_FOUND
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(INTEL_ITT
    REQUIRED_VARS
        Located_ITT_INCLUDE_DIRS
        Located_ITT_LIBS)

option(INTEL_SAMPLE_PERFPROF_ITT "Use ITT instrumentation for performance gathering and requires ITT libs/headers to be available" OFF)

if(INTEL_SAMPLE_PERFPROF_ITT)
    if(INTEL_ITT_FOUND)
        add_definitions(-DINTEL_SAMPLE_PERFPROF_ITT=1)

        set(INTEL_ITT_LIBS ${Located_ITT_LIBS})
        set(INTEL_ITT_INCLUDE_DIRS ${Located_ITT_INCLUDE_DIRS})

        message(STATUS "INTEL_ITT_INCLUDE_DIRS: ${INTEL_ITT_INCLUDE_DIRS}")
        message(STATUS "INTEL_ITT_LIBS: ${INTEL_ITT_LIBS}")
    else()
    endif()

else()
    add_definitions(-DINTEL_SAMPLE_PERFPROF_ITT=0)
    message(STATUS "INTEL_ITT is disabled")
endif()

