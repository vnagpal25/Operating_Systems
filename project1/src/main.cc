/**
 * Copyright 2023 CSCE 311
 * @author vnagpal
 * Main method
 */
#include <iostream>
#include <stack>

#include "project1/inc/calculator.h"
using std::cout, std::endl, std::stack;
stack<double> operand_stack;
stack<char> operator_stack;

// argc is the number of command line arguments delimited by a space(including
// the executable name) argv is the pointer containing all of the arguments
int main(int argc, char** argv) {
  // checks particular command line argument as either operator or operand and
  // pushes it to respective stack
  for (int i = 1; i < argc; ++i) {
    if (IsNumber(argv[i])) {
      // atof converts string to float
      operand_stack.push(atof(argv[i]));
    } else if (IsOperator(argv[i])) {
      /**
       * checks whether the read operation is more important(example
       * multiplication) 2 x 2 + 7: 2 gets pushed to operand, x gets pushed to
       * operators, 2 gets pushed to operand, + gets pushed to operators
       * since x is more important than +, 2x2 is calculated and pushed back to
       * operand, and then 4 + 7 is pushed back to operand stack
       */
      while (!operator_stack.empty() &&
             (OrderOpsPref(operator_stack.top()) >= OrderOpsPref(argv[i][0]))) {
        double value2 = operand_stack.top();
        operand_stack.pop();

        double value1 = operand_stack.top();
        operand_stack.pop();

        char op = operator_stack.top();
        operator_stack.pop();

        operand_stack.push(Calculate(value1, value2, op));
      }
      operator_stack.push(argv[i][0]);
    }
  }
  while (!operator_stack.empty()) {
    double value2 = operand_stack.top();
    operand_stack.pop();

    double value1 = operand_stack.top();
    operand_stack.pop();

    char op = operator_stack.top();
    operator_stack.pop();

    operand_stack.push(Calculate(value1, value2, op));
  }
  cout << operand_stack.top() << endl;
  return 0;
}
