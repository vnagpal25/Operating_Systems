// Copyright 2023 CSCE311
// Author: vnagpal
// Implementation of Parent DomainSocket class
#include "project2/inc/domain_socket.h"
DomainSocket::DomainSocket(const char *socket_path) {
  socket_path_ = std::string(socket_path);  // std::string manages char *
  sock_addr_ = {};                          // init struct (replaces memset)
  sock_addr_.sun_family = AF_UNIX;  // set to Unix domain socket (e.g. instead
                                    // of internet domain socket)
  // leaving leading null char sets abstract socket
  strncpy(sock_addr_.sun_path + 1,           // use strncpy to limit copy for
          socket_path,                       //   portability
          sizeof(sock_addr_.sun_path) - 2);  // -2 for leading/trailing
}
