
# CSCE 311 Project 2 - IPC
Written by Vansh Nagpal

makefile: compiles c/c++ source/header files using c++ standard and warning flags

  - make text-server: command in terminal compiles/links and creates an executable for a server
  - make text-client: command in terminal compiles/links and creates an executable for a client
  - make clean: command removes any *.o files and executables from the directory, leaving only source files

Parent Socket Files:
  - domain_socket.cc
    - implementation of DomainSocket parent class
    - constructor sets path of socket, as well as socket type(AF_UNIX)

  - domain_socket.h
    - definition of DomainSocket parent class
    - explicit constructor
    - socket address and socket path as members
    - header also defines kEoT(end of transmission character) and kUS(unit separator character)


Server Socket Files:

  - main_server.cc:
    - contains the main method to create/run an instance of the server
    - given a certain csv_server_socket file, if that file doesn't exist, it will create     that file, so that the client can connect to that same socket file
    - returns 0 to terminate

  - domain_socket_server.cc:
    - contains implementation of the DomainSocketServer class
    - constructor only chains the parent constructor, has no body
    - run method is the method that handles all the socket programming, including: 
      - creating the socket
      - unlinking socket from past connections
      - binding the socket to the local address
      - listens on the socket for incoming connections
      - accepts the first requested connection in the listening queue (client socket)
      - reading/parsing input from client
      - creating a string containing all of the desired output
      - writing the output to the client socket
    - helper method functionality will be explained in the header file

  - domain_socket_server.h
    - contains the definition of the DomainSocketServer class
    - constructor/run method are public, helper methods are private

Client Socket Files:

  - main_client.cc:
    - contains the main method to create/run an instance of the client
    - given a certain csv_server_socket file, if that file doesn't exist, it will create that file, so that the client can connect to that same socket file
    - checks if the user wrote an expression with mixed booleans(x,+ together)
      - terminates if so, with return code 2
    - combines the search arguments into a search string to write to the client once so its in the form(US- unit separator, EOT- end of transmission):
      "file_name<US>op_code<US>search_string_1<US>search_string_2<US>...<US>search_string_n<EOT>"
    - returns 0 to terminate

  - domain_socket_client.cc:
    - contains implementation of the DomainSocketClient class
    - constructor only chains the parent constructor, has no body
    - run method is the method that handles all the socket programming, including: 
      - creating a socket for the client to communicate to the server with
      - connects the socket to the server socket
      - sending/writing search string from main_client.cc to the server
      - reading output from the server and printing to STDOUT/STDLOG
    - all other helper methods will be described in the header file

  -domain_socket_client.h:
    - contains the definition of the DomainSocketClient class
    - constructor/run method are public, helper methods are private
