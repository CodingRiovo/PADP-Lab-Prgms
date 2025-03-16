#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0        

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n; 
    double t;

    if (rank == ROOT)
    {
        printf("Enter the number of iterations: ");
        fflush(stdout);
        scanf("%d", &n);
        t = MPI_Wtime();
    }

    MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    int local_n = n / size + (rank < (n % size));
    int count = 0;
    int red_count = 0;

    for (int i = 0; i < local_n; i++)
    {
        double x = (double)rand() / RAND_MAX; 
        double y = (double)rand() / RAND_MAX;
        double z = x * x + y * y;            
        if (z <= 1.0)
            count++;
    }

    MPI_Reduce(&count, &red_count, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT)
    {
        t = MPI_Wtime() - t;
        double pi = (double)red_count / n * 4;

        printf("Time taken: %f seconds\n", t);
        printf("Number of MPI processes: %d\n", size);
        printf("Total number of trials: %d\n", n);
        printf("Estimated value of Pi: %f\n", pi);
    }

    MPI_Finalize();
    return 0;
}
