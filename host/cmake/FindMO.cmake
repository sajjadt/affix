include(Utils)

# Find Intel OpenVX root directory
OPTION(ModelOptimizer_ROOT_DIR "Intel Model Optimizer root directory" "OFF")

if(NOT ModelOptimizer_ROOT_DIR)
    if(NOT "$ENV{ModelOptimizer_ROOT_DIR}" STREQUAL "")
        string(STRIP $ENV{ModelOptimizer_ROOT_DIR} ModelOptimizer_ROOT_DIR)
    elseif(DEFINED INTEL_CVSDK_DIR)
        set(ModelOptimizer_ROOT_DIR "${INTEL_CVSDK_DIR}/mo")
    endif()
endif()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MO
    REQUIRED_VARS
        ModelOptimizer_ROOT_DIR)
if(MO_FOUND)
    message(STATUS "ModelOptimizer_ROOT_DIR: ${ModelOptimizer_ROOT_DIR}")
else()
    message(STATUS "ModelOptimizer is not found")
endif()
