#ifndef PARALLEL_PM
#define PARALLEL_PM

#include <config.h>

#ifdef BEAUTIFY_USES_MPI
#include <mpi.h>
extern MPI_Datatype MPI_PHOTON;
extern bool g_parallel;

#ifndef PARALLEL
#define PARALLEL
#endif

#endif
#endif
