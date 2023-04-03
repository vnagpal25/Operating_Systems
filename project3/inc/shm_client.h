#ifndef _SHM_CLIENT_H_
#define _SMH_CLIENT_H_

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "shm_parent.h"
using std::string, std::vector;

class SharedMemoryClient : public SharedMemoryParent {
 public:
  explicit SharedMemoryClient(string file_path, string operation,
                              vector<string> search_args);

  void RunClient();

 private:
  string file_path_;

  struct search_info {
    string operation_;
    vector<string> search_args_;
    int thread_id;
    string search_line;
    bool desired_;
  } search_info_;

  shm_buf_* shm_map_;
  vector<string> server_lines_;
  vector<string> result_lines_;
  char read_lines_[THREAD_NUM][BUF_SIZE];
  void ConnectSemaphores();
  void ProcessServerInput();
  void Sleep();

  static void* ThreadExecute(void* ptr);
};

void ParseArgs(string* file_path, string* operation,
               vector<string>* search_args, char* argv[], int argc);
bool ContainsMixed(int argc, char* argv[]);
bool InvalidInput(int argc, char* argv[]);
bool AndSearch(string line, vector<string> search_args);
bool OrSearch(string line, vector<string> search_args);
bool Contains(vector<string> strings, string line);
void PrintVector(vector<string> to_print);
#endif  // _SHM_CLIENT_H_