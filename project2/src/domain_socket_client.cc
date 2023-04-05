// Copyright 2023 CSCE311
// Author: vnagpal
// Definition of Child DomainSocketClient class
#include "project2/inc/domain_socket_client.h"

#include <iostream>

#include "project2/inc/domain_socket.h"
using std::cerr, std::endl, std::clog, std::cout, std::string, std::strncmp;

DomainSocketClient::DomainSocketClient(const char *socket_path)
    : DomainSocket(socket_path) {}

void DomainSocketClient::run(const char *search_args) const {
  // unix domain socket created (using TCP protocol, byte stream)
  int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

  // error checking for creating socket
  if (socket_fd < 0) {
    cerr << strerror(errno) << endl;
    exit(-1);
  }

  // connects the socket referred to by the file descriptor socketfd to the
  // address specified by sock_addr_. Address and port are specified in
  // sockaddr.
  // connect() is used to connect to a remote [server]
  // address, that's why is client side, connect [read as: connect to server] is
  // used.
  int success =
      connect(socket_fd, reinterpret_cast<const sockaddr *>(&sock_addr_),
              sizeof(sock_addr_));

  // error checking for connecting socket to address
  if (success < 0) {
    cerr << strerror(errno) << endl;
    exit(-1);
  }

  clog << "SERVER CONNECTION ACCEPTED" << endl;

  // Writes to the server ONCE
  write(socket_fd, search_args, string(search_args).size());

  const size_t kRead_buffer_size = 2048;  // file size of bankloan2.csv is 1900
  char read_buffer[kRead_buffer_size];

  // reads from the server ONCE, fills read buffer
  ssize_t current_bytes_read = read(socket_fd, read_buffer, kRead_buffer_size);

  vector<string> lines;  // empty vector to contain the lines from the server

  while (true) {
    // checks if all of the last character is a kEoT; if so, exits. Otherwise
    // keeps trying to read
    if (read_buffer[current_bytes_read - 1] == kEoT) break;
    current_bytes_read = read(socket_fd, read_buffer, kRead_buffer_size);
  }

  // parses output from server into the lines vector
  parse_string(string(read_buffer), &lines);

  // prints out output in lines
  print_output(lines);

  // prints bytes read
  clog << "BYTES RECIEVED: " << current_bytes_read << endl;

  // error checking/exiting
  if (current_bytes_read == 0) {
    close(socket_fd);
  } else if (current_bytes_read < 0) {
    cerr << strerror(errno) << endl;
    exit(-1);
  }
}

void DomainSocketClient::print_read_buffer(char *read_buffer) const {
  // prints output for debugging purposes
  cout << "The following is what was read from the read buffer: " << endl;
  cout << string(read_buffer) << endl;
  cout << "******************************" << endl;
}

void DomainSocketClient::parse_string(string message,
                                      vector<string> *lines) const {
  // iterates through message(while it still has elements left)
  while (message.size()) {
    // finds the fist unit seperator
    const int index = static_cast<int>(message.find(kUS));
    if (index != static_cast<int>(string::npos)) {
      // gets the string from beginning to kUS
      string token = message.substr(0, index);

      // pushes read string to lines
      lines->push_back(token);

      message = message.substr(index + 1);  // changes the string to remove the
                                            // part just pushed to the vector
    } else {
      // last char is kEoT, so don't want to print that, no need to push it
      if (message[0] != kEoT) lines->push_back(message);
      message = "";  // while loop exit condition
    }
  }
}

void DomainSocketClient::print_output(vector<string> lines) const {
  // iterates through lines vector and prints out with proper formatting as
  // required by assignment
  for (int i = 0; i < static_cast<int>(lines.size()); i++)
    cout << (i + 1) << "\t" << lines[i] << endl;
}
