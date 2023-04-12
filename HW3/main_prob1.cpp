#include <cstdio>

#include <omp.h>

#include "func_tree.h"


int main() {

    Node* root = NULL;
    double* consts = NULL;
    double* vars = NULL;
    int n_const = 0;
    int n_vars = 0;

    build_func_tree(root, consts, vars, n_const, n_vars);
    set_array(consts, n_const, CONSTANT);
    
    double result;
    int num_calls;

    printf("\nHow many evaluations would you like to make? ");
    scanf("%d", &num_calls);


    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            for (int c = 0; c < num_calls; c++) {
                printf("Call %d: -----------------------------------------\n", c);

                // Get the input for this evaluation
                set_array(vars, n_vars, VARIABLE);

                // Evaluate the function
                result = evaluate_tree(root, consts, vars, n_const, n_vars);
                printf("The result of call %d is:f(x_vec, c_vec) =  %lf\n\n", c, result);
            }
        }
    }

    free_func_tree(root, consts, vars);
    return 0;
}
