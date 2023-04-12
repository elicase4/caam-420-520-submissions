#include "matrix_mult.h"

int cartesian2flat_row_major(int i, int j, int n_col){
    return j + i*n_col;
}

int cartesian2flat_col_major(int i, int j, int n_row){
    return i + j*n_row;
}

void matrix_vec_mult_row_major(double** M, double* v, double* result, int n_row, int n_col){
    for(int i = 0; i < n_row; i++){
        // add missing result initialization
        result[i] = 0.0;  
        for(int j = 0; j < n_col; j++){
            result[i] += M[i][j] * v[j];
        }
    }
}

void matrix_vec_mult_col_major(double** M, double* v, double* result, int n_row, int n_col){
    for(int j = 0; j < n_col; j++){
        // add missing result initialization
        for(int i = 0; i < n_row; i++){
            result[i] = 0.0;
        }
        for(int i = 0; i < n_row; i++){
            result[i] += M[i][j] * v[j];
        }
    }
}

void matrix_vec_mult_sparse(SparseMatrix M, double* v, double* result, int n){
    for(int i = 0; i < M.n; i++){
        // add missing result initialization
        for(int j = M.col_ptr[i]; j < M.col_ptr[i+1]; j++){
            result[M.row_ind[j]] = 0.0;
        }
        for(int j = M.col_ptr[i]; j < M.col_ptr[i+1]; j++){
            result[M.row_ind[j]] += M.val[j] * v[i];
        }
    }  
}
