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
long friendly_sieve(long n) {
    long limit = sqrt(n);
    long count = 0;
    int *composites = (int *)calloc(n + 1, sizeof(int));
    long *primes = (long *)malloc(limit * sizeof(long));
    long prime_count = 0;

    for (long i = 2; i <= limit; i++) {
        if (!composites[i]) {
            count++;
            primes[prime_count++] = i;
            mark_composites(composites, i * i, limit, i);
        }
    }

    for (long left = limit + 1; left <= n; left += limit) {
        long right = (left + limit - 1 < n) ? left + limit - 1 : n;
        for (long j = 0; j < prime_count; j++) {
            long p = primes[j];
            long start = ((left + p - 1) / p) * p;
            if (start < left)
                start += p;
            mark_composites(composites, start, right, p);
        }
        for (long i = left; i <= right; i++) {
            if (!composites[i]) {
                count++;
            }
        }
    }

    free(primes);
    free(composites);
    return count;
}

long parallel_sieve(long n) {
    long limit = sqrt(n);
    long count = 0;
    int *composites = (int *)calloc(n + 1, sizeof(int));
    long *primes = (long *)malloc(limit * sizeof(long));
    long prime_count = 0;

    for (long i = 2; i <= limit; i++) {
        if (!composites[i]) {
            count++;
            primes[prime_count++] = i;
            mark_composites(composites, i * i, limit, i);
        }
    }

    #pragma omp parallel
    {
        #pragma omp for reduction(+:count)
        for (long left = limit + 1; left <= n; left += limit) {
            long right = (left + limit - 1 < n) ? left + limit - 1 : n;
            for (long j = 0; j < prime_count; j++) {
                long p = primes[j];
                long start = ((left + p - 1) / p) * p;
                if (start < left)
                    start += p;
                mark_composites(composites, start, right, p);
            }
            for (long i = left; i <= right; i++) {
                if (!composites[i]) {
                    count++;
                }
            }
        }
    }

    free(primes);
    free(composites);
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
        long a = unfriendly_sieve(input[i]);
        t1 = omp_get_wtime() - t1;
        t2 = omp_get_wtime();
        long b = friendly_sieve(input[i]);
        t2 = omp_get_wtime() - t2;
        t3 = omp_get_wtime();
        long c = parallel_sieve(input[i]);
        t3 = omp_get_wtime() - t3;
        printf("%ldM\t\t%f\t%f\t%f\n", input[i] / 1000000, t1, t2, t3);
        printf("%ldM\t\t%ld\t\t%ld\t\t%ld\n", input[i] / 1000000, a, b, c);
        
    }
    return 0;
}