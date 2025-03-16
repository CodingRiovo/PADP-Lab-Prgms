#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<openacc.h>
#include<omp.h>

#define M 1000
#define N 1000
#define MAX_ITER 1000
#define TOL 1e-6

void initialise(double u[N][M],double f[N][M])
{
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<M;j++)
        {
            u[i][j] =0.0;
            f[i][j]=1.0;      
        }
    }
}

double jacobi(double u[N][M],double f[N][M])
{
    double u_new[N][M];
    double max_diff=0;
    int iter=0;
#pragma acc data copyin(f[0:N][0:M]) copy(u[0:N][0:M]) create(u_new[0:N][0:M])
    do
    {
        max_diff=0.0;

        #pragma acc parallel loop reduction(max:max_diff) collapse(2)

        for(int i=1;i<N-1;i++)
        {
            for(int j=1;j<M-1;j++)
            {
                u_new[i][j]=0.25*(u[i-1][j]+u[i+1][j]+u[i][j-1]+u[i][j+1]-f[i][j]);
                double diff=fabs(u[i][j]-u_new[i][j]);
                if(diff>max_diff)
                max_diff=diff;
            }
        }

        #pragma acc parallel loop collapse(2)

        for(int i=0;i<N;i++)
        {
            for(int j=0;j<M;j++)
            {
                u[i][j]=u_new[i][j];
            }
        }
        iter++;

    } while (max_diff>TOL && iter<MAX_ITER);
    
    return max_diff;

}

int main()
{
    double(*u)[M]=calloc(N,sizeof(*u));
    double(*f)[M]=calloc(N,sizeof(*f));

    initialise(u,f);

    double t = omp_get_wtime();
    double max=jacobi(u,f);
    t = omp_get_wtime() - t;
    
    printf("\nMax:%f\tTime:%f\n",max,t);

    return 0;
}