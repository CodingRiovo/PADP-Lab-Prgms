#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define SEED 35791246  // Fixed seed for reproducibility

int main(int argc, char *argv[])
{
    int niter = 0;
    int thread_count[] = {1, 2, 4, 8};
    int num_thread_configs = sizeof(thread_count) / sizeof(thread_count[0]);

    printf("Enter the number of iterations used to estimate pi: ");
    scanf("%d", &niter);

    printf("Threads\tExecution Time(s)\tPI Estimation\n");

    for (int t = 0; t < num_thread_configs; t++)
    {
        int num_threads = thread_count[t];
        int count = 0;
        double pi;
        double tt;
        omp_set_num_threads(num_threads);
        tt = omp_get_wtime();

#pragma omp parallel
        {
            int local_count = 0;
            unsigned int seed = SEED + omp_get_thread_num(); // Unique seed per thread

#pragma omp for
            for (int i = 0; i < niter; i++)
            {
                double x = (double)rand_r(&seed) / RAND_MAX;
                double y = (double)rand_r(&seed) / RAND_MAX;
                if (x * x + y * y <= 1)
                    local_count++;
            }

#pragma omp atomic
            count += local_count;
        }

        pi = (double)count / niter * 4;
        tt = omp_get_wtime()- tt;

        printf("%d\t%f\t\t%f\n", num_threads, tt, pi);
    }
    return 0;
}