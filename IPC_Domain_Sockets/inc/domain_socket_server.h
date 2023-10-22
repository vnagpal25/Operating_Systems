// Copyright 2023 CSCE311
// Author: vnagpal
// Definition of Child DomainSocketServer class
#ifndef PROJECT2_INC_DOMAIN_SOCKET_SERVER_H_
#define PROJECT2_INC_DOMAIN_SOCKET_SERVER_H_
#include <string>
#include <vector>

#include "domain_socket.h"
using std::string, std::vector;
class DomainSocketServer : public DomainSocket {
 public:
  /**
   * ctor for DSS class
   *
   * @param socket_path specifies the path for which the socket will be created
   * in
   */
  explicit DomainSocketServer(const char *socket_path);

  /**
   * run method handles all socket programming(described in detail in README)
   */
  void run() const;

 private:
  /**
   * Method parses input from client into separate vectors and string
   *
   * @param search_string singular string from client containing file_path,
   * search arguments, and operations(+ or x)
   * @param search_args pass by reference pointer to vector of strings, will
   * contain all of the search arguments as its elements
   * @param ops pass by reference pointer to vector of operations, will contain
   * all of the operations sent from client as its elements. All of its elements
   * should be the same, given proper error checking client side for mixed
   * boolean operations
   * @param file_location pass by reference to string, that will contain the
   * file path of the csv file to be searched
   */
  void parse_args(string search_string, vector<string> *search_args,
                  vector<char> *ops, string *file_location) const;

  /**
   * Method prints the search arguments sent by the client in comma/space
   * delimited fashion
   *
   * @param search_tokens vector containing all of the search arguments sent by
   * the client
   */
  void print_seeking(vector<string> search_tokens) const;

  /**
   * Method prints the operation specified by the client
   *
   * @param vector containing the operation specified by the client(should be
   * empty if only one search argument or contain homogenous elements)
   */
  void print_operation(vector<char> operations) const;

  /**
   * Method prints the file path specified by the client
   *
   * @param file_path file path to csv file to search
   */
  void print_path(string file_path) const;

  /**
   * Method checks if all the operations specified by the client are
   * different(heterogenous) redundant as mixed operations error checking is
   * handled client side
   *
   * @param ops vector containing all operations specified by client
   * @return true if has mixed operations, false otherwise (should always return
   * false)
   */
  bool contains_mixed(vector<char> ops) const;

  /**
   * Method checks if a particular line from the file contains all of the
   * arguments specified by search_args
   *
   * @param line particular line from csv file
   * @param search_args vector containing all of the search args from client
   * @return true if the line contains all of the search arguments, false
   * otherwise
   */
  bool contains_all_and(string line, vector<string> search_args) const;

  /**
   * Method checks if a particular line from the file contains at least one of
   * the arguments specified by search_args
   *
   * @param line particular line from csv file
   * @param search_args vector containing all of the search args from client
   * @return true if the line contains at least one of the search args, false
   * otherwise
   */
  bool contains_all_or(string line, vector<string> search_args) const;
};
#endif  // PROJECT2_INC_DOMAIN_SOCKET_SERVER_H_
