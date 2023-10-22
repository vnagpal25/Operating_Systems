// Copyright 2023 CSCE311
// Author: vnagpal
// Definition of Child DomainSocketClient class
#ifndef PROJECT2_INC_DOMAIN_SOCKET_CLIENT_H_
#define PROJECT2_INC_DOMAIN_SOCKET_CLIENT_H_
#include <string>
#include <vector>

#include "domain_socket.h"
using std::vector, std::string;
class DomainSocketClient : public DomainSocket {
 public:
  /**
   * ctor for DSC class
   *
   * @param socket_path specifies the path for which the socket will be created
   * in
   */
  explicit DomainSocketClient(const char *socket_path);

  /**
   * run method handles all socket programming (described in detail in README)
   */
  void run(const char *search_args) const;

 private:
  /**
   * Methods prints the read_buffer, used for debugging
   * @param read_buffer read_buffer read from server
   */
  void print_read_buffer(char *read_buffer) const;

  /**
   * Method parses the read_buffer and pushes it to lines vector
   * @param message string casted from char * read_buffer(output from server)
   * @param lines pass by reference pointer which will contain the different
   * lines from the csv as individual elements
   */
  void parse_string(string message, vector<string> *lines) const;

  /**
   * Method prints the lines in the vector parsed from parse_string
   * @param lines vector that contains all of the different lines returned from
   * the server as its elements
   */
  void print_output(vector<string> lines) const;
};
#endif  // PROJECT2_INC_DOMAIN_SOCKET_CLIENT_H_
