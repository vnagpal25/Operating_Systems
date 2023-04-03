/*Copyright 2023 CSCE311 Project 3
 * author - vnagpal
 * Contains definition of shared memory client class
 */
#ifndef PROJECT3_INC_SHM_CLIENT_H_
#define PROJECT3_INC_SMH_CLIENT_H_

#include <cstring>   // cstring functionality
#include <iostream>  // I/O c++
#include <string>    // string functionality
#include <vector>    // vector functionality

#include "shm_parent.h"          // parent functionality
using std::string, std::vector;  // readability
class SharedMemoryClient : public SharedMemoryParent {
 public:
  /**
   * Constructor for Client
   * @param file_path file to be searched
   * @param operation operation of search (and/x, or/+)
   * @param search_args vector of arguments that will be used to search the file
   */
  explicit SharedMemoryClient(string file_path, string operation,
                              vector<string> search_args);

  /**
   * Run method, calls helper method to implement all client functionality
   */
  void RunClient();

 private:
  string file_path_;  // file to be searched

  // struct that contains all information/criteria a pthread will need to search
  // the file
  struct search_info {
    string operation_;  // operation of search (and/x, or/+)

    // vector of arguments that will be used to search the file
    vector<string> search_args_;

    // id of the thread that is searching local memory
    int thread_id_;

    // line that is being checked
    string search_line_;

    // bool specifies is line being searched meets search criteria, false by
    // default
    bool desired_ = false;
  } search_info_;  // instance of search_info struct

  // from parent, struct pointer for mapping shared memory on client side
  shm_buf_* shm_map_;

  // vector of the lines that satisfy the search criteria
  vector<string> result_lines_;

  // 2d array holds THREAD_NUM lines from shared memory as its rows
  char read_lines_[THREAD_NUM][BUF_SIZE];

  /**
   * Function processes information loaded into shared memory
   */
  void ProcessSharedMemory();

  /**
   * Thread execution function, implements searching algorithm
   * @param ptr void* that will be used to pass in an search_info*
   * @return a void* that will be the same as the param
   */
  static void* ThreadExecute(void* ptr);

  /**
   * Function connects the client to the semaphores created by the
   * server
   */
  void ConnectSemaphores();

  /**
   * Puts Client to sleep, and wakes up client.
   * Used after client is done processing shared memory so that the server can
   * load in more shared memory
   */
  void Sleep();

  /**
   * Function prints out the contents of result_lines_ to the console
   */
  void PrintResults();
};

// NON CLASS FUNCTIONS
/**
 * Method parses input from user into separate vectors and strings
 * @param argc number of arguments
 * @param argv all arguments given to client via terminal
 * @param search_args pass by reference pointer to vector of strings, will
 * contain all of the search arguments as its elements
 * @param operation pass by reference pointer to string specifying operation of
 * search
 * @param file_path pass by reference to string, will contain the file path of
 * the csv file to be searched
 */
void ParseArgs(string* file_path, string* operation,
               vector<string>* search_args, char* argv[], int argc);

/**
 * Method checks if the operations given in the command line are homogenous(i.e.
 * all + or all x)
 * @param argc number of arguments
 * @param argv all arguments given to client via terminal
 * @return false if homogenous operations, true otherwise
 */
bool ContainsMixed(int argc, char* argv[]);

/**
 * Method checks if the order of arguments is arg, op, arg, op, arg
 * @param argc number of arguments
 * @param argv all arguments given to client via terminal
 * @return false if in specified format(arg, op, arg, op, arg, etc, etc.), true
 * otherwise
 */
bool InvalidOrder(int argc, char* argv[]);

/**
 * Method checks if a particular line from the file contains all of the
 * arguments specified by search_args
 *
 * @param line particular line from csv file
 * @param search_args vector containing all of the search args from client
 * @return true if the line contains all of the search arguments, false
 * otherwise
 */
bool AndSearch(string line, vector<string> search_args);

/**
 * Method checks if a particular line from the file contains at least one of
 * the arguments specified by search_args
 *
 * @param line particular line from csv file
 * @param search_args vector containing all of the search args from client
 * @return true if the line contains at least one of the search args, false
 * otherwise
 */
bool OrSearch(string line, vector<string> search_args);

/**
 * Method checks if the vector strings contains line
 * @param string vector to be checked
 * @param line line to be used for checking
 * @return true if the vector contains line, false otherwise
 */
bool Contains(vector<string> strings, string line);

/**
 * Method prints the contents of a vector to the console, used for debugging
 */
void PrintVector(vector<string> to_print);
#endif  // PROJECT3_INC_SHM_CLIENT_H_
