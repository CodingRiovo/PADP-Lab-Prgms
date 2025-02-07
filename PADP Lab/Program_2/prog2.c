#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
    int thread_cnt[] = {1, 2, 4, 8};
    int thread_cfgs = sizeof(thread_cnt) / sizeof(thread_cnt[0]);
    unsigned int dim;
    printf("Enter the dimensions of the matrix: ");
    scanf("%d", &dim);
    int *A = (int *)malloc(dim * dim * sizeof(int));
    int *B = (int *)malloc(dim * dim * sizeof(int));
    int *C = (int *)calloc(dim * dim, sizeof(int));
    for (int i = 0; i < dim * dim; i++)
    {
        A[i] = rand() % 10;
        B[i] = rand() % 10;
    }
    printf("Threads\tDimensions\tExecution Time\n");

    for (int t = 0; t < thread_cfgs; t++)
    {
        omp_set_num_threads(thread_cnt[t]);
        double time = omp_get_wtime();
#pragma omp parallel
        {

#pragma omp for
            for (int i = 0; i < dim; i++)
            {
                for (int j = 0; j < dim; j++)
                {
                    for (int k = 0; k < dim; k++)
                    {
                        C[i * dim + j] = A[i * dim + k] * B[k * dim + j];
                    }
                }
            }
        }
        time = omp_get_wtime() - time;
        printf("%d\t%d\t%f\n", thread_cnt[t], dim, time);
    }
    return 0;
}