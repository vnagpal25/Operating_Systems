#ifndef _SHM_PARENT_H_
#define _SHM_PARENT_H_

#define SHM_NAME "CSV_SHM"
#define PROD_SEM "PRODUCER_SEMAPHORE"
#define CONS_SEM "CONSUMER_SEMAPHORE"
#define BUF_SIZE 1024     // max length of shared memory segment
#define MAX_PATH_SIZE 32  // safely assuming max size of file path as 32 bytes
#define THREAD_NUM 4      // number of threads client side
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
class SharedMemoryParent {
 protected:
  // struct which will be used to map in shared memory
  struct shm_buf_ {
    char file_path[MAX_PATH_SIZE];
    size_t path_length;
    char file_lines[THREAD_NUM][BUF_SIZE];
    size_t num_lines;  // number of lines sent over
  };
  int shm_fd_;           // file descriptor for shared memory segment
  sem_t* prod_sem_ptr_;  // pointer to producer semaphore
  sem_t* cons_sem_ptr_;  // pointer to consumer semaphore
  virtual void Sleep() = 0;
};
#endif  // _SHM_PARENT_H_
