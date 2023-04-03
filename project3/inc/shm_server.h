#ifndef _SHM_SERVER_H_
#define _SHM_SERVER_H_

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "shm_parent.h"
using std::string;
class SharedMemoryServer : public SharedMemoryParent {
 public:
  explicit SharedMemoryServer();
  void RunServer();

 private:
  void CreateSemaphores();
  void DestroySemaphores();
  void ProcessFile();
  void Sleep();
  string file_path_;
  shm_buf_* shm_map_;
};
#endif  // _SHM_SERVER_H_
