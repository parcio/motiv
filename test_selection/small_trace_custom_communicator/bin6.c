#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int data;
    int *recvbuf;
    MPI_Group world_group, group_0;
    MPI_Comm comm_0;

    // Init für MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Gruppe mit allen Ranks
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    // Gruppe aus Rank 0 und 1
    int n = 2;
    int ranks[] = {0, 1};
    MPI_Group_incl(world_group, n, ranks, &group_0);

    // Kommunikator
    MPI_Comm_create(MPI_COMM_WORLD, group_0, &comm_0);

    data = rank * 10;

    if (rank == 0 || rank == 1)
    {
    
        // Ranks 0 und 1 bzw. Gruppe 0 macht eine kollektive Kommunikation        
        recvbuf = malloc(n * sizeof(int));
        MPI_Allgather(&data, 1, MPI_INT, recvbuf, 1, MPI_INT, comm_0);

        printf("Rank %d received: ", rank);
        for (int i = 0; i < n; i++)
        {
            printf("%d ", recvbuf[i]);
        }
        printf("\n");

        free(recvbuf);
        MPI_Group_free(&group_0);
    }
    else if (rank == 2)
    {
        // P2P-Komm. von Rank 2 zu 3
        int dest = 3;
        int tag = 0;
        MPI_Send(&data, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        printf("Rank %d sent: %d to rank %d\n", rank, data, dest);
    }
    else if (rank == 3)
    {
        // P2P-Komm. von Rank 3 zu 2
        int source = 2;
        int tag = 0;
        MPI_Status status;
        MPI_Recv(&data, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        printf("Rank %d received: %d from rank %d\n", rank, data, source);
    }
    else
    {
        printf("Rank %d did nothing\n", rank);
    }

    MPI_Group_free(&world_group);
    MPI_Finalize();

    return 0;
}

