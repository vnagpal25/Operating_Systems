
#include "project3/inc/shm_server.h"

#include <fstream>
#include <iostream>
#include <string>

#include "project3/inc/shm_parent.h"
using std::cout, std::clog, std::endl, std::string, std::ifstream;
SharedMemoryServer::SharedMemoryServer() {
  // attempts to destroy semaphores before creating them, ensuring proper
  // synchronization between server and client
  DestroySemaphores();

  // creates semaphores that will be used to synchronize client and server
  CreateSemaphores();
}

void SharedMemoryServer::RunServer() {
  clog << "SERVER STARTED" << endl;
  cout << "\033[1;31mbold red text\033[0m\n" << endl;
  // calls down on its own semaphore, puts itself to sleep, client will wake it
  // up
  sem_wait(prod_sem_ptr_);

  clog << "CLIENT REQUEST RECEIVED" << endl;

  shm_fd_ = shm_open(SHM_NAME, O_RDWR, 0);

  if (shm_fd_ < 0) {
    clog << "couldn't open shared memory properly" << endl;
    exit(errno);
  }
  clog << "\tMEMORY OPEN" << endl;
  shm_map_ = static_cast<shm_buf_*>(mmap(nullptr, sizeof(*shm_map_),
                                         PROT_READ | PROT_WRITE, MAP_SHARED,
                                         shm_fd_, 0));
  file_path_ = string(shm_map_->file_path, shm_map_->path_length);
  ProcessFile();
  clog << "processed file" << endl;
  Sleep();

  if (munmap(shm_map_, sizeof(*shm_map_)) == -1) {
    std::cerr << "Failed to unmap shared memory object" << std::endl;
    exit(errno);
  }
  close(shm_fd_);
  clog << "\tMEMORY CLOSED" << endl;

  DestroySemaphores();
}

void SharedMemoryServer::Sleep() {
  // wakes up the client semaphore, calling up on it
  sem_post(cons_sem_ptr_);

  // puts itself to sleep, calls down on itself
  sem_wait(prod_sem_ptr_);
}
void SharedMemoryServer::CreateSemaphores() {
  prod_sem_ptr_ =
      sem_open(PROD_SEM, O_CREAT, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR, 0);
  cons_sem_ptr_ =
      sem_open(CONS_SEM, O_CREAT, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR, 0);
}

void SharedMemoryServer::DestroySemaphores() {
  sem_unlink(PROD_SEM);
  sem_unlink(CONS_SEM);
}

void SharedMemoryServer::ProcessFile() {
  clog << "\tOPENING: " << file_path_ << endl;
  ifstream file_reader(file_path_);
  if (!file_reader) {
    const char* error_message = "INVALID FILE";
    memcpy(shm_map_->file_path, error_message, strlen(error_message));
    shm_map_->path_length = strlen(error_message);

    Sleep();
  } else {
    // at this point, server has the file path of the file to be searched, and
    // now needs to work on:
    // 1. loading in 4 lines at a time to the shared memory
    // 2. after 4 lines, put itself to sleep and wake up client

    string read_line;  // individual line that will be read
    int line_num = 1;  // line number

    // loops through the entire file line by line
    while (getline(file_reader, read_line)) {
      // fills up lines 0, 1, 2, .., THREAD_NUM -1 in file_lines** with each
      // line that is read
      // after the THREAD_NUM spots are filled, then server will call down on
      // its semaphore and call up on the client's semaphore
      memcpy(shm_map_->file_lines[(line_num - 1) % THREAD_NUM],
             read_line.c_str(), strlen(read_line.c_str()));

      // once THREAD NUM LINES have been sent over, wake up client
      if (line_num % THREAD_NUM == 0) Sleep();

      line_num++;
    }
    file_reader.close();
    const char* done_reading = "FILE CLOSED";
    Sleep();  // calling sleep here to make sure the client gets the last few
              // lines that were less than THREAD_NUM
    memcpy(shm_map_->file_path, done_reading, strlen(done_reading));
    shm_map_->path_length = strlen(done_reading);
    clog << "\t" << done_reading << endl;
  }
}