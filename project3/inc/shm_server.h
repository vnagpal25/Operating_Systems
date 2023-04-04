/*Copyright 2023 CSCE311 Project 3
 * author - vnagpal
 * Contains definition of server shared memory class
 */
#ifndef PROJECT3_INC_SHM_SERVER_H_
#define PROJECT3_INC_SHM_SERVER_H_

#include <cstring>   // cstring functionality
#include <iostream>  // IO
#include <string>    // string functionality
#include <vector>    // vector functinality

#include "shm_parent.h"  // parent functionality

// for readability
using std::string;
class SharedMemoryServer : public SharedMemoryParent {
 public:
  /**
   * Constructor for server, calls helper methods for semaphores
   */
  SharedMemoryServer();

  /**
   * Main run method, main flow of logic, calls helper methods
   * @param print_welcome prevents "SERVER STARTED" from being printed each time
   * a client connects
   */
  void RunServer(bool print_welcome);

 private:
  // file to be searched
  string file_path_;

  // from parent, struct pointer for mapping shared memory on server side
  shm_buf_* shm_map_;

  /**
   * Method creates semaphores for consumer and producer
   */
  void CreateSemaphores();

  /**
   * Method destroys semaphores for consumer and producer
   */
  void DestroySemaphores();

  /**
   * Opens the file for processing and writes contents to shared memory
   */
  void ProcessFile();

  /**
   * Puts Server to sleep, and wakes up client.
   * Used after server is done processing processing part of the file so that
   * the client can read it
   */
  void Sleep();
};
#endif  // PROJECT3_INC_SHM_SERVER_H_
