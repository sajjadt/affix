option(INTEL_SAMPLE_PERFPROF_STDOUT_DEBUG "Use stdout to print out performance numbers" ON)
if(INTEL_SAMPLE_PERFPROF_STDOUT_DEBUG)
    add_definitions(-DINTEL_SAMPLE_PERFPROF_STDOUT_DEBUG=1)
else()
    add_definitions(-DINTEL_SAMPLE_PERFPROF_STDOUT_DEBUG=0)
endif()
