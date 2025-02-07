#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

void mark_composites(int *composites, long start, long stop, long step)
{
    for (long i = start; i <= stop; i += step)
    {
        composites[i] = 1;
    }
}

long unfriendly_sieve(long n)
{
    long count = 0;
    long limit = sqrt(n);
    int *composites = (int *)calloc(n + 1, sizeof(int));
    for (long i = 2; i <= limit; i++)
    {
        if (!composites[i])
        {
            mark_composites(composites, i * i, n, i);
        }
    }
    for (long i = 2; i <= n; i++)
    {
        if (!composites[i])
        {
            count++;
        }
    }
    free(composites);
    return count;
}

long friendly_sieve(long n)
{
    long limit = sqrt(n);
    long count = 0;
    int *small_comps = (int *)calloc(limit + 1, sizeof(int));
    for (int i = 2; i * i <= limit; i++)
    {
        if (!small_comps[i])
        {
            mark_composites(small_comps, i * i, limit, i);
        }
    }
    long *primes = (long *)malloc(limit * sizeof(long));
    long prime_cnt = 0;
    for (int i = 2; i <= limit; i++)
    {
        if (!small_comps[i])
        {
            primes[prime_cnt++] = i;
            count++;
        }
    }
    free(small_comps);
    for (long start = limit + 1; start <= n; start += limit)
    {
        long end = (start + limit - 1 < n) ? start + limit - 1 : n;
        int *segment = (int *)calloc(limit, sizeof(int));

        for (long i = 0; i < prime_cnt; i++)
        {
            mark_composites(segment, ((start + primes[i] - 1) / primes[i]) * primes[i] - start, primes[i], end - start);
        }

        for (long i = 0; i < end - start; i++)
        {
            if (!segment[i])
            {
                count++;
            }
        }

        free(segment);
    }
    free(primes);
    return count;
}

long parallel_sieve(long n)
{
    long limit = sqrt(n);
    long count = 0;
    int *small_comps = (int *)calloc(limit + 1, sizeof(int));
    for (int i = 2; i * i <= limit; i++)
    {
        if (!small_comps[i])
        {
            mark_composites(small_comps, i * i, limit, i);
        }
    }
    long *primes = (long *)malloc(limit * sizeof(long));
    long prime_cnt = 0;
    for (int i = 2; i <= limit; i++)
    {
        if (!small_comps[i])
        {
            primes[prime_cnt++] = i;
            count++;
        }
    }
    free(small_comps);
#pragma omp parallel
    {
#pragma omp for
        for (long start = limit + 1; start <= n; start += limit)
        {
            long end = (start + limit - 1 < n) ? start + limit - 1 : n;
            int *segment = (int *)calloc(limit, sizeof(int));
            long local_cnt = 0;
            for (long i = 0; i < prime_cnt; i++)
            {
                mark_composites(segment, ((start + primes[i] - 1) / primes[i]) * primes[i] - start, primes[i], end - start);
            }

            for (long i = 0; i < end - start; i++)
            {
                if (!segment[i])
                {
                    local_cnt++;
                }
            }

            free(segment);
#pragma omp atomic
            count += local_cnt;
        }
    }
    free(primes);
    return count;
}

int main()
{
    long input[] = {1000000, 10000000, 100000000};
    double t1, t2, t3;
    printf("Input Size|Unfriendly Sieve|Friendly Sieve|Parallel Sieve\n");
    for (int i = 0; i < 3; i++)
    {
        t1 = omp_get_wtime();
        unfriendly_sieve(input[i]);
        t1 = omp_get_wtime() - t1;
        t2 = omp_get_wtime();
        friendly_sieve(input[i]);
        t2 = omp_get_wtime() - t2;
        t3 = omp_get_wtime();
        parallel_sieve(input[i]);
        t3 = omp_get_wtime() - t3;
        printf("%dM\t%f\t%f\t%f\n", input[i] / 1000000, t1, t2, t3);
    }
    return 0;
}