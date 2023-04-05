// Copyright 2023 CSCE311
// Author: vnagpal
// Main method implementation for server socket
#include <fstream>

#include "project2/inc/domain_socket_server.h"
using std::ifstream, std::ofstream;
const char kSocket_path[] = "";
void touch(char* file_name) {
  ifstream file(file_name);
  if (!file) ofstream new_file(file_name);
}
int main(int argc, char* argv[]) {
  if (argc != 2) return 1;
  touch(argv[1]);
  DomainSocketServer server(kSocket_path);
  server.run();
  return 0;
}
