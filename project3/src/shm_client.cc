#include "project3/inc/shm_client.h"

#include <algorithm>
#include <iostream>
#include "project3/inc/shm_parent.h"
using std::cout, std::cerr, std::endl, std::memcpy, std::find;
SharedMemoryClient::SharedMemoryClient(string file_path, string operation,
                                       vector<string> search_args) {
  file_path_ = file_path;
  search_info_.operation_ = operation;
  search_info_.search_args_ = search_args;

  shm_unlink(SHM_NAME);
  shm_fd_ = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0660);
  if (shm_fd_ < 0) {
    cerr << "FAILED TO CREATE SHARED MEMORY" << endl;
    exit(errno);
  }

  ConnectSemaphores();
}
void SharedMemoryClient::RunClient() {
  // sets the size of the shared memory segment
  ftruncate(shm_fd_, BUF_SIZE);

  // maps the shared memory to local memory, allowing client process to access
  // it as if it was its own
  shm_map_ = static_cast<shm_buf_*>(mmap(nullptr, sizeof(*shm_map_),
                                         PROT_READ | PROT_WRITE, MAP_SHARED,
                                         shm_fd_, 0));

  // Write to the shared memory object
  memcpy(shm_map_->file_path, file_path_.c_str(), strlen(file_path_.c_str()));
  shm_map_->path_length = strlen(file_path_.c_str());

  Sleep();

  // thread stuff goes here
  ProcessServerInput();

  // Unmaps the unmap shared memory object, frees up resources locally
  if (munmap(shm_map_, sizeof(*shm_map_)) == -1) {
    std::cerr << "Failed to unmap shared memory object" << std::endl;
    exit(errno);
  }

  // Close the file descriptor for the shared memory object, destroys it
  close(shm_fd_);

  // Remove the shared memory object from the system, no process will be able to
  // access it now
  if (shm_unlink(SHM_NAME) == -1) {
    cerr << "FAILED TO UNLINK SHM" << endl;
    exit(errno);
  }
}

void* SharedMemoryClient::ThreadExecute(void* ptr) {
  search_info* thread_info = static_cast<search_info*>(ptr);
  if (thread_info->operation_ == "+") {
    if (OrSearch(thread_info->search_line, thread_info->search_args_))
      return &(thread_info->search_line);

  } else if (thread_info->operation_ == "x") {
    if (AndSearch(thread_info->search_line, thread_info->search_args_))
      return &(thread_info->search_line);
  }
}

void SharedMemoryClient::ConnectSemaphores() {
  prod_sem_ptr_ = sem_open(PROD_SEM, 0, 0, 0);
  cons_sem_ptr_ = sem_open(CONS_SEM, 0, 0, 0);
}
void SharedMemoryClient::Sleep() {
  // wakes up the producer the semaphore, calling up on it
  sem_post(prod_sem_ptr_);

  // puts itself to sleep, calls down on itself
  sem_wait(cons_sem_ptr_);
}
void SharedMemoryClient::ProcessServerInput() {
  string file_message = string(shm_map_->file_path, shm_map_->path_length);

  if (file_message == "INVALID FILE") {
    cerr << file_message << endl;
    return;
  }
  while (string(shm_map_->file_path, shm_map_->path_length) != "FILE CLOSED") {
    for (int i = 0; i < THREAD_NUM; i++) {
      // if (!(find(server_lines_.begin(), server_lines_.end(),
      //            (shm_map_->file_lines[i])) != server_lines_.end()))
      //   server_lines_.push_back((shm_map_->file_lines[i]));
      // memcpy(read_lines_[i], shm_map_->file_lines[i],
      //        strlen(shm_map_->file_lines[i]));

      pthread_t threads[THREAD_NUM];
      search_info_.thread_id = i;
      search_info_.search_line = shm_map_->file_lines[i];
      pthread_create(&threads[i], nullptr, ThreadExecute, &search_info_);
    }

    // after it has pushed THREAD_NUM lines back to the vector, wake up the
    // server again
    Sleep();
  }
}
bool AndSearch(string line, vector<string> search_args) {
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

bool OrSearch(string line, vector<string> search_args) {
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

void ParseArgs(string* file_path, string* operation,
               vector<string>* search_args, char* argv[], int argc) {
  // first step is to extract file_path from argv, argv[1] is the file path
  *file_path = argv[1];

  // second step is to extract the operations from argv, operations will be
  // argv[2k+1] for integer k. At this point, the code has already checked if
  // all the operations are the same. Thus we will just extract the first
  // instance of the operation(argv[3])
  *operation = argv[3];

  // third step is to extract all of the arguments, which occur at indices 2k
  // for k >= 1
  for (int i = 2; i < argc; i += 2) search_args->push_back(argv[i]);
}

bool ContainsMixed(int argc, char* argv[]) {
  // following code checks if all even indexed elements in array after index 4
  // are different operation(either + or x) if yes, returns true if no, returns
  // false
  for (int i = 5; i < argc; i += 2) {
    if ((argv[i][0] == '+' || argv[i][0] == 'x') && (argv[i][0] != argv[3][0]))
      return true;
  }
  return false;
}

bool InvalidInput(int argc, char* argv[]) {
  for (int i = 3; i < argc; i += 2)
    if (!(argv[i][0] == '+' || argv[i][0] == 'x')) return true;
  return false;
}

void PrintVector(vector<string> to_print) {
  for (int i = 0; i < to_print.size(); i++) {
    cout << to_print[i] << endl;
  }
}