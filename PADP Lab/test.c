#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include<openacc.h>

#define M 1000
#define TOL 1e-6
#define MAX_ITER 1000

void initialize(double u[M][M], double f[M][M]){
    for(int i=0; i<M; i++){
        for(int j=0; j<M; j++){
            u[i][j] = 0.0;
            f[i][j] = 1.0;
        }
    }
}

double jacobi(double u[M][M], double f[M][M]){
    double u_new[M][M];
    double max_diff;
    int iter = 0;

    #pragma acc data copyin(f[0:M][0:M]) copy(u[0:M][0:M]) create(u_new[0:M][0:M])
    do{
        max_diff = 0;
        #pragma acc parallel loop reduction(max:max_diff) collapse(2)
        for(int i=1; i<M-1; i++){
            for(int j=1; j<M-1; j++){
                u_new[i][j] = 0.25*(u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1] - f[i][j]);
                double diff = fabs(u_new[i][j] - u[i][j]);
                if(diff > max_diff)
                    max_diff = diff;
            }
        }

        #pragma acc parallel loop collapse(2)
        for(int i=0; i<M; i++){
            for(int j=0; j<M; j++){
                u[i][j] = u_new[i][j];
            }
        }

        iter++;
    }while(iter<MAX_ITER && max_diff > TOL);
    return max_diff;
}

int main(){
    double(*u)[M] = calloc(M, sizeof(*u));
    double(*f)[M] = calloc(M, sizeof(*f));

    initialize(u, f);
    double t = omp_get_wtime();
    double ans = jacobi(u, f);
    t = omp_get_wtime() - t;

    printf("Max Diff: %f, Time: %f\n", ans, t);
    return 0;
}