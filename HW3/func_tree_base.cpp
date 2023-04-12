#include <cmath>
#include <cstdio>
#include <queue>
#include <stack>

#include "func_tree.h"


// Helper functions for building the tree =================================================
bool isOperator(char character) {
    switch (character) {
        case '-':
        case '+':
        case '/':
        case '*':
        case '(':
        case ')':
        case '^':
            return true;
        default:
            return false;
    }
}

int get_operator_score(Operator op) {
    switch (op) {
        case '(': return 0;
        case ')': return 1;
        case '-':
        case '+': return 2;
        case '/':
        case '*': return 3;
        case '^': return 4;
        default: return 0;
    }
}
// Whether op1 has strictly greater precedence over op2
// Operator predence: (, ), + or -, / or *, ^
bool hasPrecedence(Operator op1, Operator op2) {
    if( op1 == '(')
        return false;

    return get_operator_score(op1) > get_operator_score(op2);
}

// Gets input from the user sans whitespace and invalid characters
std::queue<char> get_function_string(int &n_const, int &n_vars) {
    printf("Enter the function expression: ");
    std::queue<char> input;

    int max_const_i= -1;
    int max_var_i = -1;

    char character;
    int index;
    scanf("%c", &character);
    while( character != '\n') {
        if( isOperator(character) )
            input.push(character);
        else if( character == CONSTANT ) {
            scanf("%d", &index);

            if(index > max_const_i)
                max_const_i = index;

            input.push(character);
            input.push((char) index);
        } else if( character == CONSTANT || character == VARIABLE ) {
            scanf("%d", &index);
            if(index > max_var_i)
                max_var_i = index;
                
            input.push(character);
            input.push((char) index);
        }
        scanf("%c", &character);
    }

    n_const = max_const_i + 1;
    n_vars = max_var_i + 1;

    return input;
}


std::stack<char> input2prefix(int &n_const, int &n_vars) {
    std::stack<char> operator_stack;
    std::stack<Operator> operand_stack;

    // Get input from the user
    std::queue<char> input = get_function_string(n_const, n_vars);

    // Process the input
    std::stack<char> output;
    while( !input.empty() ) {

        if( input.front() == CONSTANT || input.front() == VARIABLE ) {
            // Push the start delimiter
            operand_stack.push('[');

            // Push the identifier
            operand_stack.push(input.front());
            input.pop();

            // Push the index 
            operand_stack.push(input.front());
            input.pop();

            // Push the end delimiter
            operand_stack.push(']');
        } else if ( isOperator(input.front()) && 
                    ( input.front() == '(' || operator_stack.empty() || hasPrecedence(input.front(), operator_stack.top()) ) ) {
            operator_stack.push(input.front());
            input.pop();
        } else { // Empty the stack until the next opening brace or to the end of the stack
            while( !operator_stack.empty() && !hasPrecedence(input.front(), operator_stack.top()) ) {
                // Push the first operand to the output
                // Push the first operand to the output
                operand_stack.pop(); // Remove the ']'
                while(operand_stack.top() != '[') {
                    output.push(operand_stack.top());
                    operand_stack.pop();
                }
                operand_stack.pop(); // Remove the '['

                // Push the second operand to the output
                operand_stack.pop(); // Remove the ']'
                while(operand_stack.top() != '[') {
                    output.push(operand_stack.top());
                    operand_stack.pop();
                }
                operand_stack.pop(); // Remove the '['
                
                // Add the operator
                output.push(operator_stack.top());
                operator_stack.pop();

                // Push the new operand back to the operand stack
                operand_stack.push('[');
                while( !output.empty() ) {
                    operand_stack.push(output.top());
                    output.pop();
                }
                operand_stack.push(']');
            }

            if( !operator_stack.empty() && operator_stack.top() == '(' ) {
                operator_stack.pop(); // Remove the '(' from the operator stack
                input.pop(); // Remove the ')' from the input
            } else {
                operator_stack.push(input.front());
                input.pop();
            }
        }        
    }
  
    // If there is anything left on the stacks empty them
    while( !operator_stack.empty() ) {
        // Push the first operand to the output
        operand_stack.pop(); // Remove the ']'
        while(operand_stack.top() != '[') {
            output.push(operand_stack.top());
            operand_stack.pop();
        }
        operand_stack.pop(); // Remove the '['

        // Push the second operand to the output
        operand_stack.pop(); // Remove the ']'
        while(operand_stack.top() != '[') {
            output.push(operand_stack.top());
            operand_stack.pop();
        }
        operand_stack.pop(); // Remove the '['
        
        // Add the operator
        output.push(operator_stack.top());
        operator_stack.pop();

        // Push the new operand back to the operand stack
        operand_stack.push('[');
        while( !output.empty() ) {
            operand_stack.push(output.top());
            output.pop();
        }
        operand_stack.push(']');
    }

    
    // Convert the operand_stack to the final output
    while( !operand_stack.empty() ) {
        operand_stack.pop(); // Remove the ']'
        while( operand_stack.top() != '[' ) {
            output.push(operand_stack.top());
            operand_stack.pop(); // Remove the element itself
        }
        operand_stack.pop(); // Remove the '['
    }

    return output;
}

// Recursive function for building a tree
void build_func_tree(Node* &node,
                     std::stack<char> &prefix_input,
                     double* consts,
                     double* vars,
                     int n_const,
                     int n_vars) {
    // Check if there is any elements left to process
    if (prefix_input.empty() )
        return;

    node = new Node;
    // Check if this node is a leaf
    if ( prefix_input.top() == CONSTANT ) {
        prefix_input.pop(); // Move past the 'c' to the index
        node->val = &consts[(int) prefix_input.top()];
        prefix_input.pop();
        return;
    } else if ( prefix_input.top() == VARIABLE ) {
        prefix_input.pop(); // Move past the 'x' to the index
        node->val = &vars[(int) prefix_input.top()];
        prefix_input.pop();
        return;
    } else { // Else this is not a leaf node
        node->op = prefix_input.top();
    }
    
    prefix_input.pop();
    build_func_tree(node->left, prefix_input, consts, vars, n_const, n_vars);
    build_func_tree(node->right, prefix_input, consts, vars, n_const, n_vars);
}

// Recursive function for freeing a tree
void free_func_tree(Node* node) {
    if (node->left != NULL)
        free_func_tree(node->left);
    if (node->right != NULL)
        free_func_tree(node->right);

    delete node;
    node = NULL;
}

// Header file functions ===========================================================================

double apply_operator(Operator op, double arg1, double arg2) {
    switch(op) {
        case '-': return arg1 - arg2;
        case '+': return arg1 + arg2;
        case '/': return arg1 / arg2;
        case '*': return arg1 * arg2;
        case '^': return std::pow(arg1, arg2);
    }

    return 0;
}


void print_tree(Node* node) {
    if (node == NULL)
        return;

    if ( node->val != NULL) {
        printf("(%lf)", *(node->val));
        return;
    }
    
    printf("%c", node->op);
    print_tree(node->left);
    print_tree(node->right);
}

// Allocates memory for and initializes a function tree
void build_func_tree(Node* &root, double* &consts, double* &vars, int &n_const, int &n_vars) {
    // Prompt the user for the function and return the prefix version of it
    std::stack<char> prefix_input = input2prefix(n_const, n_vars);

    // Allocate arrays for the constants and variable values
    if (n_const > 0)
        consts = new double[n_const];
    
    if (n_vars > 0)
        vars = new double[n_vars];

    // Allocate and initialize the tree using the prefix string
    build_func_tree(root, prefix_input, consts, vars, n_const, n_vars);
}

// Frees the memory associated with a function tree
void free_func_tree(Node* &root, double* &consts, double* &vars) {
    delete[] consts;
    consts = NULL;

    delete[] vars;
    vars = NULL;

    free_func_tree(root);
}

// Sets the variable values
void set_array(double* val, int n, char array_symbol) {
    if( n == 0 )
        return;

    std::printf("Enter the %c values: \n", array_symbol);
    for(int i = 0; i < n; i++) {
        std::printf("%c[%d]: ", array_symbol, i);
        std::scanf("%lf", &val[i]);
    }
}