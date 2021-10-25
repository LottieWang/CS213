#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 512
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
    // t is the repeat times
    for (int t = 0; t<11; t++){
        start = omp_get_wtime(); //start time measurement
        // begin of matrix multiplication
        #pragma omp parallel shared(A,B,C) private(i, j, k, sum)
        {
            #pragma omp for 
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
        if (t > 0){
            printf("Time of computation %d: %f seconds\n", t, end-start);
            time_sum += end-start;
        }
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
    // t is the repeat times
    for (int t = 0; t<11; t++){
        start = omp_get_wtime(); //start time measurement
        // begin of Matrix Multiplication
        for (i = 0; i < N; i++) {
            #pragma omp parallel shared(i,A,B,C) private(j, k, sum)
            {
                #pragma omp for
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
        if (t>0){
            printf("Time of computation %d: %f seconds\n", t, end-start);
            time_sum += end-start;
        }
    }
    printf("Average time of computation: %f seconds\n", time_sum/10);
}

int main(int argc, char **argv) {
    int threads[] = {1,4,8,16};
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