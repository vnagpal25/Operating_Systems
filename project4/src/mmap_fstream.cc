#include "../inc/mmap_fstream.h"
#include <iostream>
using std::cout, std::cerr, std::clog, std::endl;
namespace mem_map {

// which file name is default?
fstream::fstream() { mode_ = std::ios_base::in | std::ios_base::out; }

fstream::fstream(const std::string& fname)
    : fstream(fname, std::ios_base::in | std::ios_base::out) {}

fstream::fstream(const std::string& fname, std::ios_base::openmode mode) {
  file_name_ = strdup(fname.c_str());
  cout << "file_name: " << file_name_ << endl;

}

void fstream::open(const std::string& fname) {
  open(fname, std::ios_base::in | std::ios_base::out);
}

void fstream::open(const std::string& fname, std::ios_base::openmode mode) {
  fd_ = ::open(fname.c_str(), mode, 0664);
}

void fstream::close() {
  // closing file descriptor, munmap here most likely
  ::close(fd_);
}

bool fstream::is_open() const {
  // 3 MAIN FUNCTIONS mmap, truncate, msync
}

}  // namespace mem_map
