#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "project3/inc/shm_client.h"
#include "project3/inc/shm_parent.h"
using std::cout, std::cerr, std::endl, std::string, std::vector;

int main(int argc, char* argv[]) {
  /**argv[0] = executable, argv[1] = csv_server_socket, argv[2] = path to search
 files, argv[3+] = search string
 sample input:
 ./csv-client dat/bankloan1.csv 34 + repay
 argv[0]       argv[1]         argv[2,3,4...]
 hence all args are 2, 4, 6, 8 (even indices after 2(inclusive))
 all operations are 3, 5, 7 (odd indices after 3(inclusive))
 all search args are going to be after argv[2]
**/
  if (argc < 3) return 1;
  if (ContainsMixed(argc, argv)) {
    cerr << "Mixed boolean operations not presently supported" << endl;
    return 2;
  }
  if (InvalidInput(argc, argv)) {
    cerr << "Improperly formatted arguments: operations must come between "
            "arguments"
         << endl;
    return 3;
  }

  string file_path, op;
  vector<string> search_args;
  ParseArgs(&file_path, &op, &search_args, argv, argc);
  PrintVector(search_args);
  SharedMemoryClient shm_client(file_path, op, search_args);
  shm_client.RunClient();
  return 0;
}