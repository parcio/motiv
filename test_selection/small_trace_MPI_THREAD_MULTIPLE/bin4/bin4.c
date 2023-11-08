#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <unistd.h>
#include <time.h>

void dummy_wrapper_sleep(int howlong) {
    usleep(howlong);
}

int main(int argc, char *argv[]){
    dummy_wrapper_sleep(250);
    int numprocs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int provided;
    //omp_lock_t lock;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    if (provided < MPI_THREAD_MULTIPLE) MPI_Abort(MPI_COMM_WORLD, 1);

    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);

    dummy_wrapper_sleep(250);
    
    // Rank 0 spawnt nichts...
    if (rank != 0){
        // Nur soviele Threads wie die Rank-Nummer!
        #pragma omp parallel num_threads(rank)
        {
            // Threadsnummer!
            int iam = omp_get_thread_num();
            int np = omp_get_num_threads();

            dummy_wrapper_sleep(iam*50);
            printf("Thread %d# von %d Threads aus Prozess %d# von %d auf %s\n", iam, np, rank, numprocs, processor_name);

            int tag = rank * 10 + iam;
            // Kritischer Bereich bzgl. MPI_Send (verworfen, Umstieg von 'serialized' auf 'multiple')
            //omp_set_lock(&lock);
            dummy_wrapper_sleep(iam*250);
            MPI_Send(&iam, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
            dummy_wrapper_sleep(iam*250);
            //omp_unset_lock(&lock);
	}
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    
    // Rank 0 ist alleine und emfängt und summiert
    if (rank == 0){
    	
    	dummy_wrapper_sleep(100);
        int sum = 0;
        // Wir grasen alle Prozesse/Ranks ab...
        for (int i = 1; i < numprocs; i++){
            // ...und ihre Threads (Prozesse sind hier auch mitdabei als Mainthread)
            for (int j = 0; j < i; j++){
                int threadnum;
                MPI_Status status;
                dummy_wrapper_sleep(100);
                // Empfangen einer beliebigen Nachricht von einem beliebigen Prozess
                MPI_Recv(&threadnum, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                sum += threadnum;
                printf("Zwischensumme: %d.\n", sum);
            }
        }
        printf("Gesamtsumme: %d.\n", sum); // Ausgabe der Summe
    }

    dummy_wrapper_sleep(500);
    MPI_Finalize();
    //omp_destroy_lock(&lock);
    return 0;
}

