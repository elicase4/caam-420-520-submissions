#include <cstdio>

#include "matrix_mult.h"

using std::printf;
using std::scanf;

// Helper functions
void read_vec(double* &v, int &n);
void read_matrix(double** &M, int &n_row, int &n_col);

void print_vec(double* v, int n);
void print_matrix(double** M, int n_row, int n_col);

void free_matrix(double** M, int n_row);

void run_problem4();
void run_problem5();

int main() {
    int choice;

    printf("Which problem would you like to run, 4 or 5? ");
    scanf("%d", &choice);

    if(choice == 4) 
        run_problem4();
    else if (choice == 5)
        run_problem5();
    else
        printf("An invalid choice was entered. Ending program...");

    return 0;
}

void read_vec(double* &v, int &n) {
    printf("Enter the length of the vector: ");
    scanf("%d", &n);

    // Note: you will need to free this memory in whatever code you write!
    v = new double[n];
    if( v == NULL ) {
        printf("could not allocate v\n");
    }

    printf("Enter the vector's values:\n");
    for(int i = 0; i < n; i++) {
        scanf("%lf", &v[i]);
    }
}

void read_matrix(double** &M, int &n_row, int &n_col) {
    printf("Enter the number of rows: ");
    scanf("%d", &n_row);
    printf("Enter the number of columns: ");
    scanf("%d", &n_col);
    
    printf("Printing matrix: n, m = %d, %d\n", n_row, n_col);

    M = new double*[n_row];
    for(int i = 0; i < n_row; i++){
      M[i] = new double[n_col];
    }
    
    printf("Enter the matrix's values:\n");
    
    for(int i = 0; i < n_row; i++) {
        for(int j = 0; j < n_col; j++)
            scanf("%lf", &M[i][j]);
    }
}

void print_vec(double* v, int n) {
    for(int i = 0; i < n; i++) {
        printf("%lf\n", v[i]);
    }
}

void print_matrix(double** M, int n_row, int n_col) {
    for(int i = 0; i < n_row; i++) {
        for(int j = 0; j < n_col; j++)
            printf(" %lf", M[i][j]);
        printf("\n");
    }
}


void free_matrix(double** M, int n_row) {
    for(int i = 0; i < n_row; i++)
        delete[] M[i];
        
    delete[] M;
}


void run_problem4() {
    double** M = NULL;
    double* v = NULL;
    int n = 0, m = 0;
    int n_vec = 0;
    
    read_matrix(M, n, m);
    read_vec(v, n_vec);

    print_matrix(M, n, m);
    print_vec(v, n_vec);
  
    double* result_row_major = new double[n];
    matrix_vec_mult_row_major(M, v, result_row_major, n, m);
    print_vec(result_row_major, n);

    double* result_col_major = new double[n];
    matrix_vec_mult_col_major(M, v, result_col_major, n, m);
    print_vec(result_col_major, n);
    
    free_matrix(M, n);
    delete[] v;
    delete[] result_col_major;
    delete[] result_row_major;
}

void run_problem5() {
  SparseMatrix M;
  M.n = 4;
  double val[7] = {3, 1, 1, 5, 1, 3, 2};
  M.val = val;
  int row_array[7] = {0, 2, 0, 3, 1, 2, 0};
  M.row_ind = row_array; 
  int col_array[5] = {0, 2, 4, 6, 7};
  M.col_ptr = col_array; 
  double v[4] = {1, 2, 3, 4};
  double* result = new double[4];
  matrix_vec_mult_sparse(M, v, result, 4);
  print_vec(result, 4);

  delete[] result;
}
