/*Copyright 2023 CSCE311 Project 3
 * author - vnagpal
 * Contains implementation of shared memory client class, as well as non class
 * helper functions
 */
#include "project3/inc/shm_client.h"  // class definitions

#include <algorithm>  // string searching algorithms
#include <iostream>   // I/O

#include "project3/inc/shm_parent.h"  // parent functionality

// for readability
using std::cout, std::cerr, std::endl, std::memcpy, std::find;

// static mutex to be used for synchronizing threads
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

SharedMemoryClient::SharedMemoryClient(string file_path, string operation,
                                       vector<string> search_args) {
  // initializes class members
  file_path_ = file_path;
  search_info_.operation_ = operation;
  search_info_.search_args_ = search_args;

  // 0. destroys shared memory location in case not destroyed from last run
  shm_unlink(SHM_NAME);

  // 1a. CREATES A SHARED MEMORY LOCATION
  shm_fd_ = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0660);
  if (shm_fd_ < 0) {
    cerr << "FAILED TO CREATE SHARED MEMORY" << endl;
    exit(errno);
  }

  // connects to semaphores created by server
  ConnectSemaphores();
}

void SharedMemoryClient::RunClient() {
  // 1b. INITIALIZES SHARED MEMORY WITH NECESSARY STRUCTURE
  //  sets the size of the shared memory segment
  ftruncate(shm_fd_, BUF_SIZE);
  // maps the shared memory to local memory, allowing client process to access
  // it as if it was its own
  shm_map_ = static_cast<shm_buf_*>(
      mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0));

  // 2. SENDS TEXT FILE NAME AND PATH TO SERVER
  memcpy(shm_map_->file_path, file_path_.c_str(), strlen(file_path_.c_str()));
  shm_map_->path_length = strlen(file_path_.c_str());

  // waiting for server to process file path and start to load in data to shm
  Sleep();

  // client wakes up and starts to process the lines loaded into shared memory
  ProcessSharedMemory();

  // after client is done printing results from threads, wake server back up so
  // that it can finish its process
  sem_post(prod_sem_ptr_);

  // Unmaps the shared memory object struct, frees up resources locally
  if (munmap(shm_map_, SHM_SIZE) == -1) {
    cerr << "Failed to unmap shared memory object" << endl;
    exit(errno);
  }

  // 6. DESTROYS SHARED MEMORY LOCATION
  //  Close the file descriptor for the shared memory object, destroys it
  close(shm_fd_);

  // Remove the shared memory object from the system, rendering it inaccessible
  // to all processes
  if (shm_unlink(SHM_NAME) == -1) {
    cerr << "FAILED TO UNLINK SHM" << endl;
    exit(errno);
  }
}

void* SharedMemoryClient::ThreadExecute(void* ptr) {
  // recasts the void* back to the struct which was the original parameter in
  // the pthread_create()
  search_info* thread_info = static_cast<search_info*>(ptr);

  // mutex to avoid race conditions,
  pthread_mutex_lock(&thread_mutex);

  // Entering Critical Section
  string search_line = thread_info->search_line_;

  // if operation is OR, else if operation is AND
  if (thread_info->operation_ == "+" &&
      OrSearch(search_line, thread_info->search_args_)) {
    // highlights the search terms (args) in the search string
    HighlightTerms(&search_line, thread_info->search_args_);

    // checks if no duplicates, then pushes to vector
    if (!Contains(thread_info->results_, search_line))
      thread_info->results_.push_back(search_line);
  } else if (thread_info->operation_ == "x" &&
             AndSearch(search_line, thread_info->search_args_)) {
    // highlights the search terms (args) in the search string
    HighlightTerms(&search_line, thread_info->search_args_);

    // checks if no duplicates, then pushes to vector
    if (!Contains(thread_info->results_, search_line))
      thread_info->results_.push_back(search_line);
  }

  // unlock mutex so other threads can implement search algorithm
  // Exiting Critical Section
  pthread_mutex_unlock(&thread_mutex);

  // returns same ptr
  return ptr;
}

void SharedMemoryClient::ConnectSemaphores() {
  // connects to both semaphores created by server
  prod_sem_ptr_ = sem_open(PROD_SEM, 0, 0, 0);
  cons_sem_ptr_ = sem_open(CONS_SEM, 0, 0, 0);
}

void SharedMemoryClient::Sleep() {
  // wakes up the producer semaphore, calling up on it
  sem_post(prod_sem_ptr_);

  // puts itself to sleep, calls down on consumer semaphore
  sem_wait(cons_sem_ptr_);
}

void SharedMemoryClient::ProcessSharedMemory() {
  //  checks if the client had sent the server an invalid file path, this
  //  message is stored in the file_path string in the shm struct
  string file_message = string(shm_map_->file_path, shm_map_->path_length);
  if (file_message == "INVALID FILE") {
    cerr << file_message << endl;
    return;
  }

  // server sends over message when it has closed the file, until then client
  // keeps reading. message will also be stored in file_path string
  while (string(shm_map_->file_path, shm_map_->path_length) != "FILE CLOSED") {
    for (int i = 0; i < THREAD_NUM; i++) {
      // 3. COPIES FROM SHARED MEMORY BACK TO LOCAL
      search_info_.search_line_ =
          string(shm_map_->file_lines[i], shm_map_->lines_length[i]);

      // 4. CREATES 4(THREAD_NUM) THREADS THAT WILL BE USED TO SEARCH 1/4 of
      // SHARED MEMORY LINES USING SEARCHING ALGORITHM
      // pass the thread function a reference struct(void *)
      pthread_create(&searcher_threads_[i], nullptr, ThreadExecute,
                     &search_info_);

      // joins threads back to main process
      pthread_join(searcher_threads_[i], nullptr);
    }

    // after it has searched THREAD_NUM lines, tell server to load more lines
    Sleep();
  }

  // 5. PRINTS OUT THE RESULTING LINES FROM THREADS TO STDOUT
  PrintResults(search_info_.results_);
}

void SharedMemoryClient::PrintResults(vector<string> results) {
  // prints out results, tab delimited
  for (int i = 0; i < static_cast<int>(results.size()); i++)
    cout << (i + 1) << "\t" << results[i] << endl;
}

bool AndSearch(string line, vector<string> search_args) {
  int count = 0;  // keeps track of current index

  bool found = true;  // bool for if all elements are found

  // while loop first checks if the element was found or not, then iterates
  // through args; if the line doesn't contain one of the args, then it exits
  // the loop as it is unnecessary to check anymore
  while (found && count < static_cast<int>(search_args.size())) {
    // npos is -1; if element isn't found, .find() returns -1, otherwise
    // returns index of where string is found
    if (line.find(search_args[count]) == string::npos) found = false;
    count++;
  }
  return found;
}

bool OrSearch(string line, vector<string> search_args) {
  int count = 0;  // keeps track of current index

  bool found = false;  // bool for if 1 element is found

  // while loop first checks if the element was found or not, then iterates
  // through args; if the element is found it exits the loop since it is
  // unnecessary to check anymore
  while (!found && count < static_cast<int>(search_args.size())) {
    // npos is -1; if element isn't found, .find() returns -1, otherwise
    // returns index of where string is found
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
  for (int i = 5; i < argc; i += 2)
    if ((argv[i][0] == '+' || argv[i][0] == 'x') && (argv[i][0] != argv[3][0]))
      return true;
  return false;
}

bool InvalidOrder(int argc, char* argv[]) {
  // checks if odd indices aren't operations + or x
  for (int i = 3; i < argc; i += 2)
    if ((argv[i][0] != '+') && (argv[i][0] != 'x')) return true;
  return false;
}

void PrintVector(vector<string> to_print) {
  // print method for debugging
  for (int i = 0; i < static_cast<int>(to_print.size()); i++)
    cout << to_print[i] << endl;
}

bool Contains(vector<string> strings, string line) {
  // checks if a vector contains an element, if the element doesn't have it,
  // std::find() iterates to the end of the vector.
  // so if it iterates to end of vector, returns false, true otherwise
  return (find(strings.begin(), strings.end(), line) != strings.end());
}

bool Contains(string str, string substring, int* index) {
  // sets the index of the occurrence of the substr in the str, if it isn't
  // found it will be set to string::npos(which is just -1)
  *index = str.string::find(substring);
  return *index != static_cast<int>(string::npos);
}

void HighlightTerms(string* search_line, vector<string> args) {
  // escape code for magenta
  string magenta_prefix = "\033[1;35m";

  // resumes normal formatting so rest of terminal isn't colored
  string magenta_suffix = "\033[0m";

  // iterates through arguments, and highlights them in the string one at a time
  for (int i = 0; i < static_cast<int>(args.size()); i++) {
    // initialize index of occurrence for safety
    int index = -1;

    // only highlights if the string has the argument
    if (Contains(*search_line, args[i], &index)) {
      // inserting prefix directly before substring
      search_line->insert(index, magenta_prefix);

      // inserting suffix directly after substring, offset due to prefix
      // inserted
      search_line->insert(index + magenta_prefix.size() + args[i].size(),
                          magenta_suffix);
    }
  }
}
