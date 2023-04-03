// Copyright 2023 CSCE311
// Author: vnagpal
// Main method implementation for client socket
#include <fstream>
#include <iostream>
#include <string>

#include "domain_socket.h"
#include "domain_socket_client.h"
using std::string, std::endl, std::cout, std::cerr, std::ifstream,
    std::ofstream;
const char *kSocket_path = "";

void touch(char *file_name) {
  ifstream file(file_name);
  if (!file) ofstream new_file(file_name);
}
char **shift(char *array[], int kSize) {
  char **shifted_args = new char *[kSize - 3];
  for (int i = 3; i < kSize; i++) shifted_args[i - 3] = array[i];
  return shifted_args;
}

string combine(char *array[], int kSize) {
  string ret_str = "";
  for (int i = 2; i < kSize - 1; i++) ret_str += (string(array[i]) + kUS);
  ret_str += (string(array[kSize - 1]));
  return ret_str;
}

void print(char *array[], int kSize) {
  for (int i = 0; i < kSize; i++) {
    cout << array[i] << " ";
  }
  cout << endl;
}
bool contains_mixed(int size, char *array[]) {
  // following code checks if all even indexed elements in array after index 4
  // are different operation(either + or x) if yes, returns true if no, returns
  // false if size is less than 4, returns false
  if (size < 4) return false;
  for (int i = 6; i < size; i += 2) {
    if ((array[i][0] == '+' || array[i][0] == 'x') &&
        (array[i][0] != array[4][0]))
      return true;
  }
  return false;
}
bool improper_formatting(int size, char *array[]) {
  for (int i = 4; i < size; i += 2)
    if (array[i][0] != '+' && array[i][0] != 'x') return true;
  return false;
}
int main(int argc, char *argv[]) {
  /**argv[0] = executable, argv[1] = csv_server_socket, argv[2] = path to search
   files, argv[3+] = search string
   sample input:
   ./text-client csv_server_sock dat/bankloan1.csv 34 + repay
   argv[0]       argv[1]         argv[2]           argv[3,4,5]
   hence all operations are 4, 6, 8 (even indices after 4)
   all search strings are 3, 5, 7 (odd indices after 3)
   all search args are going to be after argv[3]
  **/
  if (argc < 4) return 1;
  if (contains_mixed(argc, argv)) {
    cerr << "Mixed boolean operations not presently supported" << endl;
    return 2;
  }
  if (improper_formatting(argc, argv)) {
    cerr << "Improperly formatted arguments: operations must come between "
            "arguments"
         << endl;
    return 3;
  }
  touch(argv[1]);
  string search_arg_string = combine(argv, argc);
  const char *search_arg_to_send = search_arg_string.c_str();
  DomainSocketClient client(kSocket_path);
  client.run(search_arg_to_send);
  return 0;
}
