/*Copyright 2023 CSCE311 Project 3
 * author - vnagpal
 * Contains definition of abstract shared memory parent class
 */
#ifndef PROJECT3_INC_SHM_PARENT_H_
#define PROJECT3_INC_SHM_PARENT_H_

#define SHM_NAME "CSV_SHM"             // name of shared mem segment
#define PROD_SEM "PRODUCER_SEMAPHORE"  // name of server semaphore
#define CONS_SEM "CONSUMER_SEMAPHORE"  // name of client semaphore
#define BUF_SIZE 1024                  // max length of shared memory segment
#define MAX_PATH_SIZE 32  // safely assuming max size of file path as 32 bytes
#define THREAD_NUM 4      // number of threads on client side
#define SHM_SIZE 4096   // number of bytes that shm will be using, safe estimate
#include <fcntl.h>      // O-flags for semaphores and shared memory
#include <pthread.h>    // all thread functionality
#include <semaphore.h>  // all named semaphore functionality
#include <sys/mman.h>   // shared memory functionality and page flag constants
#include <unistd.h>     // for truncating/closing shared memory

class SharedMemoryParent {
 protected:
  // struct which will be used to map in shared memory on both server and client
  // side
  struct shm_buf_ {
    // used to store file_path, as well as status messages from server(INVALID
    // FILE, FILE CLOSED)
    char file_path[MAX_PATH_SIZE];

    // length of the path
    size_t path_length;

    // lines of file that will be written to shm
    char file_lines[THREAD_NUM][BUF_SIZE];

    // stores the length of each line
    size_t lines_length[THREAD_NUM];
  };
  int shm_fd_;           // file descriptor for shared memory segment
  sem_t* prod_sem_ptr_;  // pointer to producer semaphore
  sem_t* cons_sem_ptr_;  // pointer to consumer semaphore

  /**
   * Function is meant to call down on its own semaphore, and up on the opposite
   * semaphore. Virtual because will be implemented by client and server in
   * opposite fashions
   */
  virtual void Sleep() = 0;
};
#endif  // PROJECT3_INC_SHM_PARENT_H_
