#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    int niter = 0;

    int thread_count[] = {1, 2, 4, 8};
    int num_thread_configs = sizeof(thread_count) / sizeof(thread_count[0]);

    printf("Enter the no of iterations used to estimate pi:");
    scanf("%d", &niter);

    printf("Threads\tExecution Time(s)\tPI Estimation\n");
    for (int t = 0; t < num_thread_configs; t++)
    {
        int num_threads = thread_count[t];
        int count = 0;
        double pi;
        double st, end;
        omp_set_num_threads(num_threads);
        st = omp_get_wtime();
#pragma omp parallel
        {
            int local_count = 0;
#pragma omp for
            for (int i = 0; i < niter; i++)
            {
                double x = (double)rand() / RAND_MAX;
                double y = (double)rand() / RAND_MAX;
                double z = x * x + y * y;
                if (z <= 1)
                    local_count++;
            }
#pragma omp atomic
            count += local_count;
        }
        pi = (double)count / niter * 4;
        end = omp_get_wtime();

        printf("%d\t%f\t%g\n", num_threads, end - st, pi);
    }
    return 0;
}