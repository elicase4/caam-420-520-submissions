#ifndef MATRIX_MULT_H
#define MATRIX_MULT_H

struct SparseMatrix {
    int n;        // The number of columns and rows
    int nnz;      // The number of non-zeros
    int* col_ptr; // Length: n (i.e. the number of columns)
    int* row_ind; // Length: num_nonzeros
    double* val;  // Length: num_nonzeros
};

// For Problem 4:
int cartesian2flat_row_major(int i, int j, int n_col);
int cartesian2flat_col_major(int i, int j, int n_row);

/* Calculates result = M*v where:
   - M : n x n matrix
   - v : vector of size n
   - result : vector of size n
*/
void matrix_vec_mult_row_major(double** M, double* v, double* result, int n_row, int n_col);
void matrix_vec_mult_col_major(double** M, double* v, double* result, int n_row, int n_col);

// For Problem 5:
void matrix_vec_mult_sparse(SparseMatrix M, double* v, double* result, int n);
#endif
