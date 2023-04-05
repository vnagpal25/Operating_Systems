/**
 * Copyright 2023 CSCE 311
 * @author vnagpal
 * Calculator Method Implementations
*/
#include "project1/inc/calculator.h"
bool IsOperator(char* op_pointer) {
  char pot_op = *op_pointer;  // pot_op = potential operator
  return pot_op == 'x' || pot_op == '+' || pot_op == '-' || pot_op == '/';
}

bool IsNumber(const char* str) {
  char* endptr = nullptr;
  strtod(str, &endptr);
  // if str is not a valid double/int, endptr holds non null value
  return !(*endptr != '\0' || endptr == str);
}

int OrderOpsPref(char op) {
  if (op == '+' || op == '-')
    return 1;
  else if (op == 'x' || op == '/')
    return 2;
  return 0;  // never reached but safety
}

double Calculate(double a, double b, char op) {
  switch (op) {
    case '+':
      return a + b;
    case '-':
      return a - b;
    case 'x':
      return a * b;
    case '/':
      return a / b;
    default:
      return 0;  // never reached but safety
  }
}
