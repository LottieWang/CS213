#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 512
#define CHUNKSIZE 16
void MM_outer(int n_thread){
    omp_set_num_threads(n_thread);//set number of threads here
    int i, j, k;
    double sum;
    double start, end; // used for timing
    double time_sum=0;
    double A[N][N], B[N][N], C[N][N];
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = j*1;
            B[i][j] = i*j+2;
            C[i][j] = j-i*2;
        }
    }
    int chunk = CHUNKSIZE;
    for (int t = 0; t<10; t++){
        start = omp_get_wtime(); //start time measurement
        #pragma omp parallel shared(A,B,C,chunk) private(i, j, k, sum)
        {
            #pragma omp for schedule(static, chunk)
            for (i = 0; i < N; i++) {
                for (j = 0; j < N; j++) {
                    sum = 0;
                    for (k=0; k < N; k++) {
                        sum += A[i][k]*B[k][j];
                    }
                    C[i][j] = sum;
                }
            }
        }
        
        end = omp_get_wtime(); //end time measurement
        printf("Time of computation %d: %f seconds\n", t, end-start);
        time_sum += end-start;
    }
    printf("Average time of computation: %f seconds\n", time_sum/10);
}

void MM_middle(int n_thread){
    omp_set_num_threads(n_thread);//set number of threads here
    int i, j, k;
    double sum;
    double start, end; // used for timing
    double time_sum=0;
    double A[N][N], B[N][N], C[N][N];
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = j*1;
            B[i][j] = i*j+2;
            C[i][j] = j-i*2;
        }
    }
    int chunk = CHUNKSIZE;
    for (int t = 0; t<10; t++){
        start = omp_get_wtime(); //start time measurement
        for (i = 0; i < N; i++) {
            #pragma omp parallel for schedule(static, chunk) private(j, k, sum)
            for (j = 0; j < N; j++) {
                sum = 0;
                for (k=0; k < N; k++) {
                    sum += A[i][k]*B[k][j];
                }
                C[i][j] = sum;
            }
        }
        end = omp_get_wtime(); //end time measurement
        printf("Time of computation %d: %f seconds\n", t, end-start);
        time_sum += end-start;
    }
    printf("Average time of computation: %f seconds\n", time_sum/10);
}

int main(int argc, char **argv) {
    int threads[4] = {1,4,8,16};
    if (argc < 2) {
        printf("Usage: ./MM [0:outer parall mm; 1: middle parallel mm]");
        return 0;
    }
    if (atoi(argv[1]) == 0){
        for (int i = 0; i<4; i++){
            printf("Outer Parallel Matrix Multiplication: %d threads\n", threads[i]);
            MM_outer(threads[i]);
        }
    }else{
        for (int i = 0; i<4; i++){
            printf("Middle Parallel Matrix Multiplication: %d threads\n", threads[i]);
            MM_middle(threads[i]);
        }
    }
    return 0;
}