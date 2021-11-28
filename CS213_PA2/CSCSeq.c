#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct CSC{
    int n1;  // number of raw
    int n2;  // number of columns
    int m;  // number of non zero elements
    int* raw_index;     // m
    int* col_index;     // n2+1
    double* value;      // m
};

struct element{
    int i;
    int j;
    double value;
};

void swap_pt(double** A, double** B){
    double* temp = *A;
    *A = *B;
    *B = temp;
}

int comp (const void * elem1, const void * elem2) 
{
    struct element f = *((struct element *) elem1);
    struct element s = *((struct element *) elem2);
    if (f.j > s.j) return  1;
    if (f.j == s.j && f.i > s.i) return 1;
    if (f.j == s.j && f.i < s.i) return -1;
    if (f.j < s.j) return -1;
    return 0;
}

int readCSC(const char* filename, struct CSC * csc){
    FILE* input_file = fopen(filename, "r");
    if (input_file==NULL){printf("no such file.\n");return 1;}
    fscanf(input_file, "%d %d %d ",&(csc->n1),&(csc->n2),&(csc->m));
    struct element * e_list = calloc(csc->m ,sizeof(struct element));
    
    int i_index, j_index;
    double v;

    for (size_t i = 0; i<csc->m; i++){
        fscanf(input_file, "%d %d %lf ", &i_index, &j_index, &v);
        struct element e;
        e.i = i_index; e.j=j_index; e.value = v;
        e_list[i] = e;
    }
    fclose(input_file);

    qsort(e_list, csc->m , sizeof(struct element), comp);

    csc->col_index = calloc((csc->n2)+1 ,sizeof(int));
    csc->raw_index = calloc((csc->m), sizeof(int));
    csc->value = calloc((csc->m), sizeof(double));

    for (size_t i = 0; i<csc->m; i++){
        csc->raw_index[i] = e_list[i].i -1; // index from 1
        csc->col_index[e_list[i].j]++;
        csc->value[i] = e_list[i].value;
    }
    // scan
    for (size_t i = 1; i< (csc->n2)+1; i++){
        csc->col_index[i] = csc->col_index[i-1]+csc->col_index[i];
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

void MatrixVecMul(struct CSC matrix, double* x, double* y){
    for (size_t i = 0; i<matrix.n2; i++){y[i]=0;}
    for (size_t j = 0; j< matrix.n2; j++){
        for (size_t offset = matrix.col_index[j]; offset < matrix.col_index[j+1]; offset++){
            int i = matrix.raw_index[offset];
            y[i] += matrix.value[offset]*x[j];
        }
    }
}





int main(int argc, char** argv){
    if (argc < 4) {printf("usage: ./spmv matrixFileName m n\n"); return 0;}
    char* InputName = argv[1];
    int n_iters = atoi(argv[2]);
    int n_threads = atoi(argv[3]);
    bool print_output;

    // generate the output file name
    char OutName[20];
    sprintf(OutName, "CSCVec%c.txt",InputName[strlen(InputName)-5]);
    double start, end; // used for timing
    // read the matrix in CSC format
    struct CSC matrix;
    if (readCSC(InputName, &matrix) != 0) {printf("read file fail\n"); return 0;}
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
    writeArray(OutName, y, matrix.n2);
    printf("Time of Squential CSC SpMV: %f seconds\n", end-start);
    return 0;
}