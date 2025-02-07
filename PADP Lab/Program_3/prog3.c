#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <string.h>

void mark_composites(int *composite, long start, long step, long limit)
{
    for (long i = start; i <= limit; i += step)
        composite[i] = 1;
}

// Cache Unfriendly Sieve
long sieve_unfriendly(long n)
{
    long count = 0;
    int *composite = calloc(n + 1, sizeof(int));
    long limit = sqrt(n);

    for (long i = 2; i <= limit; i++)
        if (!composite[i])
            mark_composites(composite, i * i, i, n);

    for (long i = 2; i <= n; i++)
        if (!composite[i])
            count++;

    free(composite);
    return count;
}

// Cache Friendly Sieve
long sieve_friendly(long n)
{
    long limit = sqrt(n), count = 0;
    int *small_comp = calloc(limit + 1, sizeof(int));

    for (long i = 2; i * i <= limit; i++)
        if (!small_comp[i])
            mark_composites(small_comp, i * i, i, limit);

    long *primes = malloc(limit * sizeof(long)), prime_count = 0;
    for (long i = 2; i <= limit; i++)
        if (!small_comp[i])
            primes[prime_count++] = i, count++;

    free(small_comp);

    for (long start = limit + 1; start <= n; start += limit)
    {
        long end = (start + limit - 1 < n) ? start + limit - 1 : n;
        int *segment = calloc(limit, sizeof(int));

        for (long i = 0; i < prime_count; i++)
            mark_composites(segment, ((start + primes[i] - 1) / primes[i]) * primes[i] - start, primes[i], end - start);

        for (long i = 0; i <= end - start; i++)
            if (!segment[i])
                count++;

        free(segment);
    }
    free(primes);
    return count;
}

// Parallel Sieve
long sieve_parallel(long n)
{
    long limit = sqrt(n), count = 0, n_factor = 0;
    long *factor = malloc(limit * sizeof(long));
    int *composite = calloc(limit + 1, sizeof(int));

    for (long i = 2; i <= limit; i++)
        if (!composite[i])
        {
            count++;
            mark_composites(composite, i * i, i, limit);
            factor[n_factor++] = i;
        }

#pragma omp parallel
    {
        int *local_comp = calloc(limit + 1, sizeof(int));
        long *marker = malloc(n_factor * sizeof(long));

#pragma omp for reduction(+ : count)
        for (long start = limit + 1; start <= n; start += limit)
        {
            memset(local_comp, 0, (limit + 1) * sizeof(int));
            long end = (start + limit - 1 < n) ? start + limit - 1 : n;

            for (long k = 0; k < n_factor; k++)
                marker[k] = (start + factor[k] - 1) / factor[k] * factor[k] - start;

            for (long k = 0; k < n_factor; k++)
                mark_composites(local_comp, marker[k], factor[k], end - start);

            for (long i = 0; i <= end - start; i++)
                if (!local_comp[i])
                    count++;
        }
        free(local_comp);
        free(marker);
    }

    free(composite);
    free(factor);
    return count;
}

int main()
{
    long input[] = {1000000, 10000000, 100000000};

    printf("\nSieve of Eratosthenes - Prime Number Counting\n");
    printf("+---------------+-----------------+-----------------+-----------------+\n");
    printf("|   Input Size  |  Unfriendly (s) |  Friendly (s)  |  Parallel (s)   |\n");
    printf("+---------------+-----------------+-----------------+-----------------+\n");

    for (int i = 0; i < 3; i++)
    {
        long n = input[i];
        double start, time1, time2, time3;

        start = omp_get_wtime();
        long res1 = sieve_unfriendly(n);
        time1 = omp_get_wtime() - start;

        start = omp_get_wtime();
        long res2 = sieve_friendly(n);
        time2 = omp_get_wtime() - start;

        start = omp_get_wtime();
        long res3 = sieve_parallel(n);
        time3 = omp_get_wtime() - start;

        printf("| %11ldM | %8.6f s  | %8.6f s  | %8.6f s  |\n", n / 1000000, time1, time2, time3);
    }

    printf("+---------------+-----------------+-----------------+-----------------+\n");
    return 0;
}
