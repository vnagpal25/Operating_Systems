#include <cstring>
#include <iostream>

#include "project3/inc/shm_parent.h"
#include "project3/inc/shm_server.h"
using std::cout, std::clog, std::endl;
int main(int argc, char* argv[]) {
  SharedMemoryServer shm_server;
  shm_server.RunServer();
  return 0;
}