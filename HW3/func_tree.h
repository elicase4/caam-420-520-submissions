#ifndef FUNCTION_TREE_H
#define FUNCTION_TREE_H
#include <cstdio>

const char CONSTANT = 'c';
const char VARIABLE = 'x';
const int MAX_NUM_INDEX_DIGITS = 3;

typedef char Operator;

struct Node {
    // For the tree structure
    Node* parent = NULL;
    Node* left = NULL;
    Node* right = NULL;
    
    // For operations
    Operator op;
    double* val = NULL;
};

double apply_operator(Operator op, double arg1, double arg2);

// Allocates memory for and initializes a function tree
void build_func_tree(Node* &root, double* &consts, double* &vars, int &n_const, int &n_vars);

// Frees the memory associated with a function tree
void free_func_tree(Node* &root, double* &consts, double* &vars);

// Sets the variable values using values read from the user/command line
void set_array(double* val, int n, char array_symbol);

// For testing
void print_tree(Node* root);

// Evaluates a function tree
double evaluate_tree(Node* root, double* consts, double* vars, int n_const, int n_vars);

#endif
