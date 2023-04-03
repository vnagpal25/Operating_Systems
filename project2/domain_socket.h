// Copyright 2023 CSCE 311
// Author: vnagpal
// Definition of Parent DomainSocket class
#ifndef _DOMAIN_SOCKET_H_
#define _DOMAIN_SOCKET_H_
#include <sys/socket.h>  // Unix header for sockets, using socket
#include <sys/un.h>  // defns for Unix domain sockets, using struct sockaddr_un
#include <unistd.h>  // Unix standard header, using close

#include <cassert>  // using assert
#include <cerrno>   // using errno
#include <cstddef>  // using size_t
#include <cstdlib>  // exit
#include <cstring>  // using strncpy, strerror
#include <iostream>
#include <string>
const char kUS = static_cast<char>(31);
const char kEoT = static_cast<char>(3);
class DomainSocket {
 public:
  explicit DomainSocket(const char *socket_path);

 protected:
  sockaddr_un sock_addr_;    // socket address from sys/un.h
  std::string socket_path_;  // let std::string manage char *};
};
#endif  // _DOMAIN_SOCKET_H_
