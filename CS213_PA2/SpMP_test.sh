#!/bin/bash

# gcc -fopenmp CSRSeq.c -o CSRSeq
# spmv=CSRSeq
# echo "-------------------"
# echo "CSR Sequential Test"
# for M in 1 2 3; do
#     echo "**** Matrix $M ****"
#     for t in {0..10}; do
#         ./$spmv matrix$M.txt 8 1
#     done
# done


# gcc -fopenmp CSROpenMP.c -o CSROpenMP
# spmv=CSROpenMP
# for M in 1 2 3; do
#     for n_thread in 1 2 4 8 16; do
#         echo "-------------------"
#         echo "CSR OpenMP Test with $n_thread threads"
#         echo "**** Matrix $M ****"
#         for t in {0..10}; do
#             ./$spmv matrix$M.txt 8 $n_thread
#         done
#     done
# done


gcc -fopenmp CSCSeq.c -o CSCSeq
spmv=CSCSeq
echo "-------------------"
echo "CSC Sequential Test"
for M in 1 2 3; do
    echo "**** Matrix $M ****"
    for t in {0..10}; do
        ./$spmv matrix$M.txt 8 1
    done
done


gcc -fopenmp CSCOpenMP.c -o CSCOpenMP
spmv=CSCOpenMP
for M in 1 2 3; do
    for n_thread in 1 2 4 8 16; do
        echo "-------------------"
        echo "CSC OpenMP Test with $n_thread threads"
        echo "**** Matrix $M ****"
        for t in {0..10}; do
            ./$spmv matrix$M.txt 8 $n_thread
        done
    done
done

