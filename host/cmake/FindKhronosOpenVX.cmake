include(Utils)

# Unset find locations
unset(KhronosOpenVX_INCLUDE_DIR CACHE)
unset(KhronosOpenVX_OPENVX_LIB CACHE)
unset(KhronosOpenVX_VXU_LIB CACHE)
unset(KhronosOpenVX_RUNTIME_DIR CACHE)

# Find Khronos implementation root directory
set(KhronosOpenVX_ROOT_DIR_DESCRIPTION "Khronos sample OpenVX implementation root directory")
set(KhronosOpenVX_ROOT_DIR "KhronosOpenVX_ROOT_DIR-NOTFOUND" CACHE PATH "${KhronosOpenVX_ROOT_DIR_DESCRIPTION}")
if(NOT KhronosOpenVX_ROOT_DIR)
    if(DEFINED ENV{KhronosOpenVX_DIR})
        set(KhronosOpenVX_ROOT_DIR "$ENV{KhronosOpenVX_DIR}" CACHE PATH "${KhronosOpenVX_ROOT_DIR_DESCRIPTION}" FORCE)
    elseif(DEFINED ENV{KHRONOS_SAMPLE_IMPL_DIR})
        set(KhronosOpenVX_ROOT_DIR "$ENV{KHRONOS_SAMPLE_IMPL_DIR}" CACHE PATH "${KhronosOpenVX_ROOT_DIR_DESCRIPTION}" FORCE)
    endif()
endif()

# Find Khronos implementation include directory
util_find_dir(KhronosOpenVX_INCLUDE_DIR
    FILES
        VX/vx.h
        VX/vxu.h
        VX/vx_api.h
        VX/vx_kernels.h
        VX/vx_nodes.h
        VX/vx_types.h
        VX/vx_vendors.h
    PATHS "${KhronosOpenVX_ROOT_DIR}/include"
    DOC "Khronos OpenVX sample implementation include directory")

# Find Khronos OpenVX libraries
set(lib_dir "${KhronosOpenVX_ROOT_DIR}/lib")
find_library(KhronosOpenVX_OPENVX_LIB openvx HINTS "${KhronosOpenVX_ROOT_DIR}/lib" "${KhronosOpenVX_ROOT_DIR}/bin" NO_DEFAULT_PATH)
find_library(KhronosOpenVX_VXU_LIB    vxu    HINTS "${KhronosOpenVX_ROOT_DIR}/lib" "${KhronosOpenVX_ROOT_DIR}/bin" NO_DEFAULT_PATH)

# Set runtime libraries directory
util_find_dir(KhronosOpenVX_RUNTIME_DIR
    FILES
        "${CMAKE_SHARED_LIBRARY_PREFIX}openvx${CMAKE_SHARED_LIBRARY_SUFFIX}"
        "${CMAKE_SHARED_LIBRARY_PREFIX}vxu${CMAKE_SHARED_LIBRARY_SUFFIX}"
    PATHS "${KhronosOpenVX_ROOT_DIR}/bin"
    DOC "Khronos OpenVX sample implementation runtime libraries directory")

# Handle find_package() arguments, and set IPP_FOUND
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KhronosOpenVX
    FOUND_VAR KhronosOpenVX_FOUND
    REQUIRED_VARS
        KhronosOpenVX_INCLUDE_DIR
        KhronosOpenVX_OPENVX_LIB KhronosOpenVX_VXU_LIB
        KhronosOpenVX_RUNTIME_DIR)
mark_as_advanced(
    KhronosOpenVX_INCLUDE_DIR
    KhronosOpenVX_OPENVX_LIB KhronosOpenVX_VXU_LIB
    KhronosOpenVX_RUNTIME_DIR)

# Set output variables
if(KhronosOpenVX_FOUND)
    set(KhronosOpenVX_INCLUDE_DIRS "${KhronosOpenVX_INCLUDE_DIR}")
    set(KhronosOpenVX_LIBRARIES "${KhronosOpenVX_OPENVX_LIB}" "${KhronosOpenVX_VXU_LIB}")
    set(KhronosOpenVX_RUNTIME_LIBRARY_DIRS "${KhronosOpenVX_RUNTIME_DIR}")
    set(KhronosOpenVX_DEFINITIONS "USE_KHRONOS_OPENVX" "USE_KHRONOS_SAMPLE_IMPL" "USE_OPENVX_1_0_1")
endif()
