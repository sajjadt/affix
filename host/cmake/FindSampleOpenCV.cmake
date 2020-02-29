option(INTEL_SAMPLE_USE_OPENCV "Requires OpenCV and use it in the samples" ON)
if(INTEL_SAMPLE_USE_OPENCV)
    #get OpenCV_DIR from enviroment
    if(NOT DEFINED OpenCV_DIR AND DEFINED ENV{OpenCV_DIR})
        set(OpenCV_DIR "$ENV{OpenCV_DIR}")
    endif()

    if( NOT DEFINED OpenCV_DIR )
        if(EXISTS "${INTEL_CVSDK_DIR}/opencv/share/OpenCV" )
            set(OpenCV_DIR "${INTEL_CVSDK_DIR}/opencv/share/OpenCV")
        elseif(EXISTS "$ENV{INTEL_CVSDK_DIR}/opencv/share/OpenCV")
            set(OpenCV_DIR "$ENV{INTEL_CVSDK_DIR}/opencv/share/OpenCV")
        endif()
    endif()

    #report current OpenCV folder
    if(OpenCV_DIR)
        message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
    else()
        message(STATUS "OpenCV_DIR is not set")
    endif()

    find_package(OpenCV PATHS ${OpenCV_DIR} QUIET NO_CMAKE_PATH)
    set(OpenCV_DIR "${OpenCV_DIR}" PARENT_SCOPE)

#ifdef USE_CVE_DEVICE
    if( EXISTS ENV{CVE_SDK} )
    if( NOT OpenCV_ARCH OR NOT OpenCV_ARCH STREQUAL "x86")
        message(WARNING "CMake found some version of OpenCV, but the architecture of OpenCV (defined by OpenCV_ARCH) doesn't match 'x86', OpenCV_ARCH = ${OpenCV_ARCH}")
        unset(OpenCV_FOUND)
    endif()
    endif()
#endif

endif()

if(OpenCV_FOUND)
    message(STATUS "OPENCV is enabled")
    message(STATUS "OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS}")
    message(STATUS "OpenCV_LIBS=${OpenCV_LIBS}")
    message(STATUS "OpenCV_ARCH=${OpenCV_ARCH}")
    add_definitions(-DINTEL_SAMPLE_USE_OPENCV=1)
else()
    message(STATUS "OPENCV is disabled or not found")
    add_definitions(-DINTEL_SAMPLE_USE_OPENCV=0)
endif()
