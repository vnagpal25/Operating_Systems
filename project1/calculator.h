/**
 * Copyright 2023 CSCE 311
 * @author vnagpal
 * Calculator method definitions
 */
#ifndef PROJECT1_CALCULATOR_H_
#define PROJECT1_CALCULATOR_H_
#include <iostream>

/**
 * Method checks if a character is an operator
 * @param op_pointer is an array(unit length) holding a character
 * @return true/false if a character is +, -, x, or /
 */

bool IsOperator(char* op);

/**
 * Method checks if a string is a number or not
 * @param str is a character array/string that could be a number
 * @return true/false if its a valid floating point or integer number
 */

bool IsNumber(const char* str);

/**
 * Method checks how important an operation is(order of operations)
 * Is used to determine when to pop off operands/operations from the stack
 * @param op character which is either '+', '-', 'x', or '/'
 * @return 1 if + or -, and 2 if x or /
 */

int OrderOpsPref(char op);

/**
 * calculates results from simple arithmetic expressions
 * @param a, b two operands
 * @param op specifies operation
 * @return a @ b, where @ is the operation
 */

double Calculate(double a, double b, char op);
#endif  // PROJECT1_CALCULATOR_H_
