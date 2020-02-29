option(USE_KHRONOS_SAMPLE_IMPL "Use Khronos sample OpenVX implementation instead of Intel OpenVX 
implementation" OFF)

if(NOT USE_KHRONOS_SAMPLE_IMPL)

    find_package(IntelOpenVX)

    if(IntelOpenVX_FOUND)
        add_definitions(-DOPENVX_ACCESS_COMMIT_SCALAR=1)

        add_definitions(-DINTEL_SAMPLE_USE_OPENVX)

        set(OpenVX_INCLUDE ${IntelOpenVX_INCLUDE_DIRS})
        set(OpenVX_LIBS ${IntelOpenVX_LIBRARIES})

        message(STATUS "OpenVX_INCLUDE= " ${OpenVX_INCLUDE})
        message(STATUS "OpenVX_LIBS= " ${OpenVX_LIBS})
    else()
        message(STATUS "[ ERR ] Intel CVSDK OpenVX implementation: not found")
    endif()
else()

    message(STATUS "[ INFO ] Khronos sample OpenVX implementation is used")

    find_package(KhronosOpenVX)

    if(KhronosOpenVX_FOUND)
        add_definitions(-DUSE_KHRONOS_SAMPLE_IMPL)

        add_definitions(-DOPENVX_ACCESS_COMMIT_MATRIX=0)
        add_definitions(-DOPENVX_ACCESS_COMMIT_SCALAR=0)

        add_definitions(-DINTEL_SAMPLE_USE_OPENVX)

        set(OpenVX_INCLUDE ${KhronosOpenVX_INCLUDE_DIRS})
        set(OpenVX_LIBS ${KhronosOpenVX_LIBRARIES})

        message(STATUS "OpenVX_INCLUDE= " ${OpenVX_INCLUDE})
        message(STATUS "OpenVX_LIBS= " ${OpenVX_LIBS})
    else()
        message(STATUS "[ ERR ] Khronos sample OpenVX implementation: not found")
    endif()

endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
