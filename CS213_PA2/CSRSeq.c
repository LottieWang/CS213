#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct CSR{
    int n1;  // number of raw
    int n2;
    int m;  // number of non zero elements
    int* col_index;     // m
    int* raw_index;     // n1+1
    double* value;      // m
};

void swap_pt(double** A, double** B){
    double* temp = *A;
    *A = *B;
    *B = temp;
}

int readCSR(const char* filename, struct CSR * csr){
    FILE* input_file = fopen(filename, "r");
    if (input_file==NULL){printf("no such file.\n");return 1;}
    fscanf(input_file, "%d %d %d ",&(csr->n1),&(csr->n2),&(csr->m));
    csr->col_index = calloc(csr->m ,sizeof(int));
    csr->raw_index = calloc((csr->n1)+1, sizeof(int));
    csr->value = calloc((csr->m), sizeof(double));
    // for (size_t i = 0; i<csr.n1+1; i++){if (csr.raw_index[i]!= 0){printf("not initialized to zero\n");}}
    int i_index, j_index;
    double v;
    
    for (size_t i = 0; i<csr->m; i++){
        fscanf(input_file, "%d %d %lf ", &i_index, &j_index, &v);
        csr->raw_index[i_index]++;
        csr->col_index[i] = j_index-1; // index from 1
        csr->value[i] = v;
    }
    fclose(input_file);
    
    for (size_t i = 1; i< (csr->n1)+1; i++){
        csr->raw_index[i] = csr->raw_index[i-1]+csr->raw_index[i];
    }
    return 0;
}

int writeArray(char* name, double* y, int n){
    FILE *fp;
    fp = fopen(name, "w+");
    for (size_t i = 0; i<n; i++){
        fprintf(fp, "%lf\n", y[i]);
    }
    fclose(fp);
}

void MatrixVecMul(struct CSR matrix, double* x, double* y){
    for (size_t i = 0; i<matrix.n1; i++){
        double sum = 0;
        // printf("y[%ld]\n", i);
        for (size_t offset = matrix.raw_index[i]; offset<matrix.raw_index[i+1]; offset++){
            int j = matrix.col_index[offset];
            sum += matrix.value[offset]*x[j];
            // printf("%f * %f \n", matrix.value[offset], x[j]);
        }
        y[i]=sum;
    }
}

int main(int argc, char** argv){
    if (argc < 4) {printf("usage: ./spmv matrixFileName m n\n"); return 0;}
    char* InputName = argv[1];
    int n_iters = atoi(argv[2]);    // m
    int n_threads = atoi(argv[3]);  // n
    bool print_output;
    if (argc == 4) {
        print_output=false;
    }else{
        print_output=true;
    }
    // generate the output file name
    char OutName[20];
    sprintf(OutName, "CSRVec%c.txt",InputName[strlen(InputName)-5]);
    double start, end; // used for timing
    // read the matrix in CSR format
    struct CSR matrix;
    if (readCSR(InputName, &matrix) != 0) {printf("read file fail\n"); return 0;}
    
    // initialize the vector
    double* x = malloc(sizeof(double)*matrix.n2);
    double* y = malloc(sizeof(double)*matrix.n2);
    for (size_t i = 0; i<matrix.n2; i++){x[i]=1;}
    // calling the matrix vector multiplication
    start = omp_get_wtime(); //start time measurement
    for (int t = 0; t<n_iters; t++){
        MatrixVecMul(matrix, x, y);
        swap_pt(&x,&y);
    }
    end = omp_get_wtime(); //end time measurement
    swap_pt(&x,&y);
    if (print_output){
        writeArray(OutName, y, matrix.n2);
    }
    printf("Time of Squential CSR SpMV: %f seconds\n", end-start);
    return 0;
}