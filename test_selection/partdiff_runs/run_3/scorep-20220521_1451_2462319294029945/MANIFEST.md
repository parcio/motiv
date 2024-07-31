# Experiment directory overview

The result directory of this measurement should contain the following files:

   1. Files that should be present even if the measurement aborted:

      * `MANIFEST.md`           This manifest file.
      * `scorep.cfg`            Listing of used environment variables.

   2. Files that will be created by subsystems of the measurement core:

      * Tracing:

        * `traces.otf2`         OTF2 anchor file.
        * `traces.def`          OTF2 global definitions file.
        * `traces/`             Sub-directory containing per location trace
                                data.

# List of Score-P variables that were explicitly set for this measurement

The complete list of Score-P variables used, incl. current default values,
can be found in `scorep.cfg`.

    SCOREP_ENABLE_PROFILING
    SCOREP_ENABLE_TRACING
    SCOREP_VERBOSE
    SCOREP_TOTAL_MEMORY
    SCOREP_PAGE_SIZE
    SCOREP_EXPERIMENT_DIRECTORY
    SCOREP_OVERWRITE_EXPERIMENT_DIRECTORY
    SCOREP_MACHINE_NAME
    SCOREP_EXECUTABLE
    SCOREP_ENABLE_SYSTEM_TREE_SEQUENCE_DEFINITIONS
    SCOREP_FORCE_CFG_FILES
    SCOREP_TIMER
    SCOREP_PROFILING_TASK_EXCHANGE_NUM
    SCOREP_PROFILING_MAX_CALLPATH_DEPTH
    SCOREP_PROFILING_BASE_NAME
    SCOREP_PROFILING_FORMAT
    SCOREP_PROFILING_ENABLE_CLUSTERING
    SCOREP_PROFILING_CLUSTER_COUNT
    SCOREP_PROFILING_CLUSTERING_MODE
    SCOREP_PROFILING_CLUSTERED_REGION
    SCOREP_PROFILING_ENABLE_CORE_FILES
    SCOREP_TRACING_USE_SION
    SCOREP_TRACING_MAX_PROCS_PER_SION_FILE
    SCOREP_FILTERING_FILE
    SCOREP_SUBSTRATE_PLUGINS
    SCOREP_SUBSTRATE_PLUGINS_SEP
    SCOREP_METRIC_PAPI
    SCOREP_METRIC_PAPI_PER_PROCESS
    SCOREP_METRIC_PAPI_SEP
    SCOREP_METRIC_RUSAGE
    SCOREP_METRIC_RUSAGE_PER_PROCESS
    SCOREP_METRIC_RUSAGE_SEP
    SCOREP_METRIC_PLUGINS
    SCOREP_METRIC_PLUGINS_SEP
    SCOREP_METRIC_PERF
    SCOREP_METRIC_PERF_PER_PROCESS
    SCOREP_METRIC_PERF_SEP
    SCOREP_TOPOLOGY_PLATFORM
    SCOREP_TOPOLOGY_PROCESS
    SCOREP_TOPOLOGY_MPI
    SCOREP_MEMORY_RECORDING
    SCOREP_MPI_MAX_COMMUNICATORS
    SCOREP_MPI_MAX_WINDOWS
    SCOREP_MPI_MAX_EPOCHS
    SCOREP_MPI_MAX_GROUPS
    SCOREP_MPI_ENABLE_GROUPS
    SCOREP_MPI_MEMORY_RECORDING
    SCOREP_MPI_ONLINE_ANALYSIS
