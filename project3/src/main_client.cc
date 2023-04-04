/*Copyright 2023 CSCE311 Project 3
 * author - vnagpal
 * Contains main method for client, makes/runs client instance
 */
#include <cstring>   // cstring functionality
#include <iostream>  // IO
#include <string>    // string functionality
#include <vector>    // vector functionality

#include "project3/inc/shm_client.h"
#include "project3/inc/shm_parent.h"
using std::cout, std::cerr, std::endl, std::string, std::vector;

int main(int argc, char* argv[]) {
  /**
   argv[0] = executable, argv[1] = csv_server_socket, argv[2] = path to
   search files, argv[3+] = search string sample input:
   ./csv-client dat/bankloan1.csv 34 + repay
   argv[0]       argv[1]         argv[2,3,4...]
   hence all args are 2, 4, 6, 8 (even indices after 2(inclusive))
   all operations are 3, 5, 7 (odd indices after 3(inclusive))
   all search args are going to be after argv[2]
  */

  // error checking for proper input
  if (argc < 3) return 1;
  if (ContainsMixed(argc, argv)) {
    cerr << "Mixed boolean operations not presently supported" << endl;
    return 2;
  }
  if (InvalidOrder(argc, argv)) {
    cerr << "Improperly formatted arguments: operations must come between "
            "arguments"
         << endl;
    return 3;
  }

  // parses input from command line, into file_path, operation of search, and
  // search arguments
  string file_path, op;
  vector<string> search_args;
  ParseArgs(&file_path, &op, &search_args, argv, argc);

  // Steps 1 - 6 in constructor and run method(creates/runs client process)
  SharedMemoryClient shm_client(file_path, op, search_args);
  shm_client.RunClient();

  // 7. TERMINATES BY RETURNING 0 TO INDICATE NOMINATIVE EXIT STATUS
  return 0;
}
