# Current limitations:
# Always use 64-bit Linux libraries, ippstatic version

include(Utils)

# Unset locations in cache, if present
unset(IntelOpenVX_INCLUDE_DIR CACHE)
unset(IntelOpenVX_IAP_LIB CACHE)
unset(IntelOpenVX_IAP_OPENVX_LIB CACHE)
unset(IntelOpenVX_IAP_OPENVX_EXT_LIB CACHE)
unset(IntelOpenVX_IAP_RUNTIME_DIR CACHE)
unset(IntelOpenVX_IAP_OPENVX_RUNTIME_DIR CACHE)
unset(IntelOpenVX_IAP_OPENVX_EXT_RUNTIME_DIR CACHE)

# Find Intel OpenVX root directory
set(IntelOpenVX_ROOT_DIR_DESCRIPTION "Intel OpenVX SDK root directory")
set(IntelOpenVX_ROOT_DIR "IntelOpenVX_ROOT_DIR-NOTFOUND" CACHE PATH "${IntelOpenVX_ROOT_DIR_DESCRIPTION}")
if(NOT IntelOpenVX_ROOT_DIR)
    if(DEFINED ENV{IntelOpenVX_DIR})
        set(IntelOpenVX_ROOT_DIR "$ENV{IntelOpenVX_DIR}" CACHE PATH "${IntelOpenVX_ROOT_DIR_DESCRIPTION}" FORCE)
    elseif(DEFINED IAP_SDK_DIR)
        set(IntelOpenVX_ROOT_DIR "${IAP_SDK_DIR}" CACHE PATH "${IntelOpenVX_ROOT_DIR_DESCRIPTION}" FORCE)
    elseif(DEFINED ENV{IAP_SDK_DIR})
        set(IntelOpenVX_ROOT_DIR "$ENV{IAP_SDK_DIR}" CACHE PATH "${IntelOpenVX_ROOT_DIR_DESCRIPTION}" FORCE)
    elseif(DEFINED INTEL_CVSDK_DIR)
        set(IntelOpenVX_ROOT_DIR "${INTEL_CVSDK_DIR}" CACHE PATH "${IntelOpenVX_ROOT_DIR_DESCRIPTION}" FORCE)
    elseif(DEFINED ENV{INTEL_CVSDK_DIR})
        set(IntelOpenVX_ROOT_DIR "$ENV{INTEL_CVSDK_DIR}" CACHE PATH "${IntelOpenVX_ROOT_DIR_DESCRIPTION}" FORCE)
    endif()
endif()

#get INTEL_CVSDK_DIR from enviroment if it is not defined yet
if(NOT DEFINED INTEL_CVSDK_DIR AND DEFINED ENV{INTEL_CVSDK_DIR})
    set(INTEL_CVSDK_DIR "$ENV{INTEL_CVSDK_DIR}")
endif()
if(NOT DEFINED INTEL_CVSDK_DIR AND DEFINED ENV{IAP_SDK_DIR})
    set(INTEL_CVSDK_DIR "$ENV{IAP_SDK_DIR}")
endif()
if(NOT DEFINED INTEL_CVSDK_DIR)
    set(INTEL_CVSDK_DIR ${IntelOpenVX_ROOT_DIR})
endif()
message(STATUS "INTEL_CVSDK_DIR: ${INTEL_CVSDK_DIR}")

# Find include directory
# if we are using old build of OVX SDK
util_find_dir(IntelOpenVX_INCLUDE_DIR
    FILES
        VX/vx.h
        VX/vxu.h
        VX/vx_api.h
        VX/vx_kernels.h
        VX/vx_nodes.h
        VX/vx_types.h
        VX/vx_vendors.h
    PATHS "${IntelOpenVX_ROOT_DIR}/ovx-sdk-lin/openvx/include/" "${IntelOpenVX_ROOT_DIR}/openvx/include/" "${IntelOpenVX_ROOT_DIR}/emulator/Release/include"
    DOC "Intel OpenVX include directory")

    message(STATUS "Intel CVSDK OpenVX runtime is used")
    set(iap_lib_dirs "${IntelOpenVX_ROOT_DIR}/ovx-sdk-lin/lib/intel64-ippstatic"        "${IntelOpenVX_ROOT_DIR}/openvx/lib")
    set(ovx_lib_dirs "${IntelOpenVX_ROOT_DIR}/ovx-sdk-lin/openvx/lib/intel64-ippstatic" "${IntelOpenVX_ROOT_DIR}/openvx/lib")

    # Find IAP library
    if (WIN32)
        find_library(IntelOpenVX_IAP_LIB iap_openvx iap_openvx.lib HINTS ${iap_lib_dirs} NO_DEFAULT_PATH)
    else()
        find_library(IntelOpenVX_IAP_LIB iap libiap.so.1.8 HINTS ${iap_lib_dirs} NO_DEFAULT_PATH)
    endif()

    # Find OpenVX libraries
    if (WIN32)
        find_library(IntelOpenVX_IAP_OPENVX_LIB  iap_openvx iap_openvx.lib  HINTS ${ovx_lib_dirs} NO_DEFAULT_PATH)
    else()
        find_library(IntelOpenVX_IAP_OPENVX_LIB     iap_openvx     libiap_openvx.so.1.0     HINTS ${ovx_lib_dirs} NO_DEFAULT_PATH)
    endif()

    if(USE_INTEL_OLDER_EXT)
    find_library(IntelOpenVX_IAP_OPENVX_EXT_LIB iap_openvx_ext libiap_openvx_ext.so.1.0 HINTS ${ovx_lib_dirs} NO_DEFAULT_PATH)
    endif()

    # Set runtime libraries directory
    if (WIN32)
        find_path(IntelOpenVX_IAP_RUNTIME_DIR
            "${CMAKE_SHARED_LIBRARY_PREFIX}iap_openvx${CMAKE_STATIC_LIBRARY_SUFFIX}"
            "${CMAKE_SHARED_LIBRARY_PREFIX}iap_openvx${CMAKE_STATIC_LIBRARY_SUFFIX}.1.8"
            PATHS ${iap_lib_dirs}
            NO_DEFAULT_PATH)
        find_path(IntelOpenVX_IAP_OPENVX_RUNTIME_DIR
            "${CMAKE_SHARED_LIBRARY_PREFIX}iap_openvx${CMAKE_STATIC_LIBRARY_SUFFIX}"
            "${CMAKE_SHARED_LIBRARY_PREFIX}iap_openvx${CMAKE_STATIC_LIBRARY_SUFFIX}.1.0"
            PATHS ${ovx_lib_dirs}
            NO_DEFAULT_PATH)
    else()
        find_path(IntelOpenVX_IAP_RUNTIME_DIR
            "${CMAKE_SHARED_LIBRARY_PREFIX}iap${CMAKE_SHARED_LIBRARY_SUFFIX}"
            "${CMAKE_SHARED_LIBRARY_PREFIX}iap${CMAKE_SHARED_LIBRARY_SUFFIX}.1.8"
            PATHS ${iap_lib_dirs}
            NO_DEFAULT_PATH)
        find_path(IntelOpenVX_IAP_OPENVX_RUNTIME_DIR
            "${CMAKE_SHARED_LIBRARY_PREFIX}iap_openvx${CMAKE_SHARED_LIBRARY_SUFFIX}"
            "${CMAKE_SHARED_LIBRARY_PREFIX}iap_openvx${CMAKE_SHARED_LIBRARY_SUFFIX}.1.0"
            PATHS ${ovx_lib_dirs}
            NO_DEFAULT_PATH)
    endif()
    find_path(IntelOpenVX_IAP_OPENVX_EXT_RUNTIME_DIR
        "${CMAKE_SHARED_LIBRARY_PREFIX}iap_openvx_ext${CMAKE_SHARED_LIBRARY_SUFFIX}"
        "${CMAKE_SHARED_LIBRARY_PREFIX}iap_openvx_ext${CMAKE_SHARED_LIBRARY_SUFFIX}.1.0"
        PATHS ${ovx_lib_dirs}
        NO_DEFAULT_PATH)

    set (IntelOpenVX_TRAIT_DEFS "OPENVX_ACCESS_COMMIT_MATRIX=0" "OPENVX_ACCESS_COMMIT_SCALAR=1")

if(USE_INTEL_OLDER_EXT)
    # Handle find_package() arguments, and set IntelOpenVX_FOUND
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(IntelOpenVX
        FOUND_VAR IntelOpenVX_FOUND
        REQUIRED_VARS
            IntelOpenVX_INCLUDE_DIR
            IntelOpenVX_IAP_LIB IntelOpenVX_IAP_OPENVX_LIB IntelOpenVX_IAP_OPENVX_EXT_LIB
            IntelOpenVX_IAP_RUNTIME_DIR IntelOpenVX_IAP_OPENVX_RUNTIME_DIR IntelOpenVX_IAP_OPENVX_EXT_RUNTIME_DIR)
    mark_as_advanced(
        IntelOpenVX_INCLUDE_DIR
        IntelOpenVX_IAP_LIB IntelOpenVX_IAP_OPENVX_LIB IntelOpenVX_IAP_OPENVX_EXT_LIB
        IntelOpenVX_IAP_RUNTIME_DIR IntelOpenVX_IAP_OPENVX_RUNTIME_DIR IntelOpenVX_IAP_OPENVX_EXT_RUNTIME_DIR)

else()
    # Handle find_package() arguments, and set IntelOpenVX_FOUND
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(IntelOpenVX
        FOUND_VAR IntelOpenVX_FOUND
        REQUIRED_VARS
            IntelOpenVX_INCLUDE_DIR
            IntelOpenVX_IAP_LIB IntelOpenVX_IAP_OPENVX_LIB
            IntelOpenVX_IAP_RUNTIME_DIR IntelOpenVX_IAP_OPENVX_RUNTIME_DIR)
    mark_as_advanced(
        IntelOpenVX_INCLUDE_DIR
        IntelOpenVX_IAP_LIB IntelOpenVX_IAP_OPENVX_LIB
        IntelOpenVX_IAP_RUNTIME_DIR IntelOpenVX_IAP_OPENVX_RUNTIME_DIR)

endif()

    # Set output variables
    if(IntelOpenVX_FOUND)
        set(IntelOpenVX_INCLUDE_DIRS "${IntelOpenVX_INCLUDE_DIR}")
        set(IntelOpenVX_LIBRARIES
            "${IntelOpenVX_IAP_OPENVX_LIB}"
            "${IntelOpenVX_IAP_OPENVX_EXT_LIB}")
        set(IntelOpenVX_RUNTIME_LIBRARY_DIRS
            "${IntelOpenVX_IAP_RUNTIME_DIR}"
            "${IntelOpenVX_IAP_OPENVX_RUNTIME_DIR}"
            "${IntelOpenVX_IAP_OPENVX_EXT_RUNTIME_DIR}")
        list(REMOVE_DUPLICATES IntelOpenVX_RUNTIME_LIBRARY_DIRS)
        set(IntelOpenVX_DEFINITIONS "USE_INTEL_OPENVX" "${IntelOpenVX_TRAIT_DEFS}")
    endif()

