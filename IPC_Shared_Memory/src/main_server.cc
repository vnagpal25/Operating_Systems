/*Copyright 2023 CSCE311 Project 3
 * author - vnagpal
 * Contains main method for server, makes/runs server instance
 */
#include <cstring>   // cstring functions
#include <iostream>  // IO

#include "project3/inc/shm_parent.h"  // parent functionality
#include "project3/inc/shm_server.h"  // server functionality

// readability
using std::cout, std::clog, std::endl;
int main(int argc, char* argv[]) {
  // creates/runs server instance
  SharedMemoryServer shm_server;
  shm_server.RunServer(true);

  // exits
  return 0;
}
