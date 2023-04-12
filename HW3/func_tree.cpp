#include "func_tree.h"

#include "omp.h"

// Evaluates a function tree
double evaluate_tree(Node* root, double* consts, double* vars, int n_const, int n_vars) {
    // If we are on a leaf just return its value
    if (root->val != NULL)
        return *(root->val);
    
    // Otherwise apply the operation to the resulting values from the subtrees
    double result_left = evaluate_tree(root->left, consts, vars, n_const, n_vars);
    double result_right = evaluate_tree(root->right, consts, vars, n_const, n_vars);

    return apply_operator(root->op, result_left, result_right);
}