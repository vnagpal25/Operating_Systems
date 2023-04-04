
#include "project3/inc/shm_server.h"

#include <fstream>
#include <iostream>
#include <string>

#include "project3/inc/shm_parent.h"
using std::cout, std::cerr, std::clog, std::endl, std::string, std::ifstream;
SharedMemoryServer::SharedMemoryServer() {
  // attempts to destroy semaphores before creating them, ensuring proper
  // synchronization between server and client
  DestroySemaphores();

  // creates semaphores that will be used to synchronize client and server
  CreateSemaphores();
}

void SharedMemoryServer::RunServer(bool print_welcome) {
  // 1. SERVER STARTED
  if (print_welcome) clog << "SERVER STARTED" << endl;
  // calls down on its own semaphore, puts itself to sleep, client will wake it
  // up
  sem_wait(prod_sem_ptr_);

  // 2. SERVER HAS RECEIVED CLIENT REQUEST
  clog << "CLIENT REQUEST RECEIVED" << endl;

  // 3. OPENS SHARED MEMORY LOCATION, WRITES TO STDLOG
  if ((shm_fd_ = shm_open(SHM_NAME, O_RDWR, 0)) < 0) {
    clog << "couldn't open shared memory properly" << endl;
    exit(errno);
  }
  clog << "\tMEMORY OPEN" << endl;
  // maps shared memory to local memory using struct
  shm_map_ = static_cast<shm_buf_*>(
      mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0));

  // gets file path out of shared memory
  file_path_ = string(shm_map_->file_path, shm_map_->path_length);

  // 4.
  ProcessFile();
  // Sleep();

  if (munmap(shm_map_, SHM_SIZE) == -1) {
    cerr << "Failed to unmap shared memory object" << endl;
    exit(errno);
  }
  close(shm_fd_);
  clog << "\tMEMORY CLOSED" << endl;
  RunServer(false);
}

void SharedMemoryServer::ProcessFile() {
  // 4a, writes file path to STDLOG
  clog << "\tOPENING: " << file_path_ << endl;

  // 4b. opens file
  ifstream file_reader(file_path_);
  if (!file_reader) {
    // if not a valid file writes invalid file path message to client in the
    // file_path struct string
    const char* error_message = "INVALID FILE";
    memcpy(shm_map_->file_path, error_message, strlen(error_message));
    shm_map_->path_length = strlen(error_message);
    return;
  } else {
    string read_line;  // individual line that will be read
    int line_num = 1;  // line number

    // 4b. reads file
    //  loops through the entire file line by line
    while (getline(file_reader, read_line)) {
      /*fills up lines 0, 1, 2, .., THREAD_NUM -1 in file_lines** with each
      line that is read
       after the THREAD_NUM spots are filled, then server will call down on
       its semaphore and call up on the client's semaphore*/

      // 4c. writes contents to shared memory
      // writes the lines to shared memory
      memcpy(shm_map_->file_lines[(line_num - 1) % THREAD_NUM],
             read_line.c_str(), strlen(read_line.c_str()));
      // writes the length of each line to shared memory
      shm_map_->lines_length[(line_num - 1) % THREAD_NUM] =
          strlen(read_line.c_str());

      // once THREAD_NUM LINES have been sent over, wake up client
      if (line_num % THREAD_NUM == 0) Sleep();

      line_num++;
    }
    // 4d. closes file
    file_reader.close();
    const char* done_reading = "FILE CLOSED";
    Sleep();  // calling sleep here to make sure the client gets the last few
              // lines that were less than THREAD_NUM

    // sends message via shared memory to alert client that server is done
    // reading
    memcpy(shm_map_->file_path, done_reading, strlen(done_reading));
    shm_map_->path_length = strlen(done_reading);

    // writes to STDLOG
    clog << "\t" << done_reading << endl;
    Sleep();
  }
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
