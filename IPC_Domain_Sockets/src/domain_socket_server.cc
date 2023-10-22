// Copyright 2023 CSCE311
// Author: vnagpal
// Implementation of Child DomainSocketServer class
#include "project2/inc/domain_socket_server.h"

#include <sys/sysinfo.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "project2/inc/domain_socket.h"
using std::cerr, std::endl, std::clog, std::cout, std::string, std::strncmp,
    std::ifstream, std::vector, std::adjacent_find, std::not_equal_to;

DomainSocketServer::DomainSocketServer(const char* socket_path)
    : DomainSocket(socket_path) {}

void DomainSocketServer::run() const {
  clog << "SERVER STARTED" << endl;
  size_t kMaxConnections = get_nprocs_conf() - 1;
  clog << "\tMAX CLIENTS: " << kMaxConnections << endl;
  int socket_fd, client_socket_fd;

  // creates socket of type Unix Domain and TCP(byte stream)
  socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

  // error checking for socket
  if (socket_fd < 0) {
    cerr << strerror(errno) << endl;
    exit(-1);
  }

  // unlink() is counter to bind, removes connection of socket to file address
  unlink(socket_path_.c_str());

  // The bind() function binds a unique local name specified by sock_addr_ to
  // the socket with descriptor socket.
  // bind() associates the socket with its local address
  // that's why server side binds, so that clients can use that address to
  // connect to server.]
  int bind_success =
      bind(socket_fd, reinterpret_cast<const sockaddr*>(&sock_addr_),
           sizeof(sock_addr_));

  // error checking for binding server socket to address
  if (bind_success < 0) {
    cerr << strerror(errno) << endl;
    exit(-1);
  }

  // listen() puts the server socket in a passive mode, where it waits for the
  // client to approach the server to make a connection. The backlog, defines
  // the maximum length to which the queue of pending connections for sockfd may
  // grow. If a connection request arrives when the queue is full, the client
  // may receive an error with an indication of ECONNREFUSED.
  int success = listen(socket_fd, kMaxConnections);
  if (success < 0) {
    cerr << strerror(errno) << endl;
    exit(-1);
  }

  // prepares for a potentially 1024 size character input, 'slight' overkill
  const size_t kRead_buffer_size = 1024;
  char read_buffer[kRead_buffer_size];
  while (true) {
    // accept() extracts the first connection request on the queue of pending
    // connections for the listening socket, sockfd, creates a new connected
    // socket (client socket), and returns a new file descriptor referring to
    // that socket. At this point, the connection is established between client
    // and server, and they are ready to transfer data.
    client_socket_fd = accept(socket_fd, nullptr, nullptr);

    // error checking for accepting socket
    if (client_socket_fd < 0) {
      cerr << strerror(errno) << endl;
      continue;
    }
    clog << "CLIENT CONNECTED" << endl;

    vector<string> search_args;  // will contain all of the search arguments
    vector<char> operations;     // will contain the operations
    string file_location;        // file path

    // reads from client ONCE
    int num_bytes_search_string =
        read(client_socket_fd, read_buffer, kRead_buffer_size);
    string search_string(read_buffer, num_bytes_search_string);

    // method populates search_args, operations, and file_locations
    parse_args(search_string, &search_args, &operations, &file_location);

    print_path(file_location);

    print_operation(operations);

    print_seeking(search_args);

    ssize_t total_bytes_sent = 0;

    // opens file
    ifstream file_reader(file_location);

    // string that will be sent, using std::string for ease of concatenation,
    // will be converted to cstring later before sending
    string to_send = "";
    if (contains_mixed(operations)) {
      const char* mixed_bool =
          "Mixed boolean operations not presently supported";
      total_bytes_sent =
          write(client_socket_fd, mixed_bool, string(mixed_bool).size());
    } else {
      if (!file_reader) {
        // checks if file is valid
        string invalid_file = "INVALID FILE";
        const char* invalid = (invalid_file += kEoT).c_str();
        total_bytes_sent =
            write(client_socket_fd, invalid, invalid_file.size());
        clog << "INVALID FILE" << endl;
      } else {
        string read_line;
        // iterates through file and stores each line in read_line
        while (getline(file_reader, read_line)) {
          // and operation with multiple args
          if (operations.size() > 0 && operations[0] == 'x') {
            if (contains_all_and(read_line, search_args))
              to_send += (read_line + kUS);  // add line and kUS
          } else {
            // if operation is + or just a single argument
            if (contains_all_or(read_line, search_args))
              to_send += (read_line + kUS);  // add line and kUS
          }
        }
        to_send += kEoT;                                // adds kEoT
        const char* to_send_cstring = to_send.c_str();  // converted to cstring
        // writes to client ONCE
        total_bytes_sent = write(client_socket_fd, to_send_cstring,
                                 string(to_send_cstring).size());
        clog << "BYTES SENT: " << total_bytes_sent << endl << endl;
      }
    }
  }
}
void DomainSocketServer::parse_args(string search_string,
                                    vector<string>* search_args,
                                    vector<char>* ops,
                                    string* file_location) const {
  // next 3 lines extract the file path from the string sent by the client (file
  // path should be the first substring in the string)
  const int file_index = static_cast<int>(search_string.find("csv"));
  (*file_location) =
      search_string.substr(0, file_index + 3);  // +3 for c,s, and v
  search_string =
      search_string.substr(file_index + 4);  // +4 after removing *.csv and kUS
  // iterates through search string (while it still has elements left)
  while (search_string.size()) {
    // finds the fist unit seperator
    const int index = static_cast<int>(search_string.find(kUS));
    // npos is -1; index will be -1 if kUS isn't found
    if (index != static_cast<int>(string::npos)) {
      // gets the string from beginning to kUS
      string token = search_string.substr(0, index);
      // checks if operation and pushes to ops or search_args
      if (token == "+" || token == "x") {
        ops->push_back(token[0]);
      } else {
        search_args->push_back(token);
        // pushes back to vector everything up until delimiter
      }
      search_string =
          search_string.substr(index + 1);  // changes the string to remove the
                                            // part just pushed to the vector
    } else {
      search_args->push_back(search_string);  // kUS not found, last element
      search_string = "";                       // exit condition for while loop
    }
  }
}

void DomainSocketServer::print_seeking(vector<string> search_tokens) const {
  // writes arguments specified by client to STDLOG, comma delimited
  clog << "SEEKING: ";
  for (int i = 0; i < static_cast<int>(search_tokens.size()) - 1; i++)
    clog << search_tokens[i] << ", ";
  clog << search_tokens[search_tokens.size() - 1] << endl;  // last element
}
void DomainSocketServer::print_operation(vector<char> operations) const {
  // writes search operation to STDLOG, n/a if no operation AND/OR for x/+
  // respectively
  if (operations.size() <= 0) {
    clog << "OPERATION: n/a" << endl;
    return;
  }
  string operation = (operations[0] == '+') ? "OR" : "AND";
  clog << "OPERATION: " << operation << endl;
}
void DomainSocketServer::print_path(string file_path) const {
  // writes file_path to STDLOG
  clog << "PATH: " << file_path << endl;
}

bool DomainSocketServer::contains_mixed(vector<char> ops) const {
  // adjacent find will iterate over vector so trying to find adjacent elements
  // that are not the same if all the elements in the vector are the same, the
  // iterator reaches the end thus we are checking here to find if the iterator
  // found adjacent elements in the vector that were different before the last
  // element
  //  in this case, we know that the vector has differing elements and thus has
  //  mixed boolean operations
  if (ops.size() <= 0) return false;  // if no operations
  return adjacent_find(ops.begin(), ops.end(), not_equal_to<char>()) !=
         ops.end();
}

bool DomainSocketServer::contains_all_and(string line,
                                          vector<string> search_args) const {
  int count = 0;      // keeps track of current index
  bool found = true;  // bool for if all elements are found
  // while loop first checks if the element was found or not, then iterates
  // through args; if the line doesn't contain one of the args, then it exits
  // the loop as it is unnecessary to check anymore
  while (found && count < static_cast<int>(search_args.size())) {
    // npos is -1; if element isn't found, .find() returns -1, otherwise returns
    // index of where string is found
    if (line.find(search_args[count]) == string::npos) found = false;
    count++;
  }
  return found;
}

bool DomainSocketServer::contains_all_or(string line,
                                         vector<string> search_args) const {
  int count = 0;       // keeps track of current index
  bool found = false;  // bool for if 1 element is found
  // while loop first checks if the element was found or not, then iterates
  // through args; if the element is found it exits the loop since it is
  // unnecessary to check anymore
  while (!found && count < static_cast<int>(search_args.size())) {
    // npos is -1; if element isn't found, .find() returns -1, otherwise returns
    // index of where string is found
    if (line.find(search_args[count]) != string::npos) found = true;
    count++;
  }
  return found;
}
