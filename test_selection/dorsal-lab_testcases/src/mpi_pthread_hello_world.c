#include <pthread.h>
#include "mpi.h"
#include <stdio.h>
#include <scorep/SCOREP_User.h>

int rank, nprocs;

void *print_hello_world(int id)
{
	SCOREP_USER_REGION_DEFINE( print_hello_world_region )
	SCOREP_USER_REGION_BEGIN( print_hello_world_region, "print_hello_world", SCOREP_USER_REGION_TYPE_COMMON )
	printf(" Hello World! From Thread:%d On Process: %d. \n", id, rank);
	SCOREP_USER_REGION_END( print_hello_world_region )
	return NULL;
}

void main(int argc, char *argv[])
{
	SCOREP_USER_REGION_DEFINE( main_region )
	SCOREP_USER_REGION_BEGIN( main_region, "main", SCOREP_USER_REGION_TYPE_COMMON )
	pthread_t thread1, thread2;

	MPI_Status  status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	pthread_create(&thread1, NULL, (void *(*) (void *)) print_hello_world, (void *) 1);

	pthread_create(&thread2, NULL, (void *(*) (void *)) print_hello_world, (void *) 2);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	MPI_Finalize();
	
	SCOREP_USER_REGION_END( main_region )
	return;

}
