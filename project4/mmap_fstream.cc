#include "mmap_fstream.h"

#include <iostream>
using std::cout, std::cerr, std::clog, std::endl;
namespace mem_map {

// which file name is default?
fstream::fstream() {
  // Creates Memory-mapped file stream obj without file name; file name must be
  //   specified in call to Open.
  //
  //   You may use default parameter values to allow following constructors to
  //   absorb this one
  //
  mode_ = std::ios_base::in | std::ios_base::out;
}

fstream::fstream(const std::string& fname)
    : fstream(fname, std::ios_base::in | std::ios_base::out) {
  // Creates Memory-mapped file stream obj with file name
  //
  //   Open mode is std::ios_base::in | std::ios_base:: out by default.
  //
  //   Result of constructor can be determined by is_open
  //
  //   You may use default parameter values to allow following constructor to
  //   absorb this one
  //
}

fstream::fstream(const std::string& fname, std::ios_base::openmode mode) {
  file_name_ = strdup(fname.c_str());
  mode_ = mode;

  open(file_name_, mode_);
}

void fstream::open(const std::string& fname) {
  // Attempts to open file given by file_name
  //
  //   Open mode is std::ios_base::in | std::ios_base::out by default
  //
  //   Result can be determined by is_open
  //
  //   Does nothing if file is already open
  //
  //   You may use default parameter values to overload following method to
  //   absorb this one
  //
  //   Simplifying assumption: no file will ever be larger than 2^12 bytes
  //

  open(fname, std::ios_base::in | std::ios_base::out);
}

void fstream::open(const std::string& fname, std::ios_base::openmode mode) {
  file_name_ = strdup(fname.c_str());

  if (mode & std::ios_base::ate) {
    mode &= ~std::ios_base::ate;
    cursor_end = true;
  }

  fd_ = ::open(file_name_, convert_mode_to_Oflag(mode), 0664);
  if (fd_ < 0) {
    cerr << "couldn't open file, ::open() failed " << strerror(errno) << endl;
    exit(errno);
  }

  refresh_file_status();

  addr_ = static_cast<char*>(mmap(
      nullptr, PAGE_SIZE, convert_mode_to_PROTflag(mode), MAP_SHARED, fd_, 0));
  if (addr_ == MAP_FAILED) {
    cerr << "couldn't mmap properly " << strerror(errno) << endl;
    exit(errno);
  }

  if (cursor_end) cursor_ = size();

  if (access(file_name_, F_OK) != 0) {
    cout << "file doesn't exist " << strerror(errno) << endl;
    exit(errno);
  }
  // chmod(file_name_, 0664);
  if (::truncate(file_name_, file_status_.st_size) < 0) {
    cerr << "::truncate error " << strerror(errno) << endl;
    exit(errno);
  }
  open_ = true;
}

void fstream::close() {
  // closing file descriptor
  if (::close(fd_) < 0) {
    cerr << "couldn't close fd properly " << strerror(errno) << endl;
    exit(errno);
  }

  // unmapping memory address
  if (munmap(addr_, PAGE_SIZE) == -1) {
    cerr << "Failed to unmap file " << strerror(errno) << endl;
    exit(errno);
  }
  open_ = false;
}

bool fstream::is_open() const {
  // 3 MAIN FUNCTIONS mmap, truncate, msync
  return open_;
}

size_t fstream::size() const {
  // Returns file's current size; may change dynmically due to fstream::put
  //
  return file_status_.st_size;
}

fstream& fstream::put(char c) {
  // Writes character at "next" space in the file and updates cursor
  //
  //  This method may increase the size of a file
  //

  if (file_status_.st_size == 0 || file_status_.st_size == cursor_) {
    cout << "cursor was at the end" << endl;

    ::truncate(file_name_, file_status_.st_size + 1);
  }
  refresh_file_status();
  for(int i = 0; i < size();i++)
    cout << addr_[i] << " ";
  addr_[cursor_++] = c;

  int msync_sig = msync(addr_, strlen(file_name_), MS_SYNC);
  if (msync_sig < 0) {
    printf("msync err: %s", strerror(errno));
  }

  return *this;
}

char fstream::get() {
  // Retrieves "next" character from file and updates cursor
  //
  //  Returns '\0' when no further characters exist
  //
  //  This method must not modify a file; only updates cursor position if not
  //  at end of file
  //
  cout << "cursor: " << cursor_ << endl;
  if (cursor_ < file_status_.st_size) return addr_[cursor_++];
  return '\0';
}

int fstream::convert_mode_to_Oflag(std::ios_base::openmode mode) {
  // std::ios_base::in, open for reading, O_RDONLY
  // std::ios_base::out, open for writing, O_WRONLY
  // in | out, open for reading and writing, O_RDWR
  // std::ios_base::ate  (open with cursor at the end of the file)
  int o_flag = 0;

  if (mode == (std::ios_base::in | std::ios_base::out)) {
    o_flag |= O_RDWR;
  } else if (mode == std::ios_base::in) {
    o_flag |= O_RDONLY;
  } else if (mode == std::ios_base::out) {
    o_flag |= O_WRONLY;
  }

  if (access(file_name_, F_OK) != 0) o_flag |= O_CREAT;
  return o_flag;
}

int fstream::convert_mode_to_PROTflag(std::ios_base::openmode mode) {
  if (mode == (std::ios_base::in | std::ios_base::out)) {
    return PROT_READ | PROT_WRITE;
  } else if (mode == std::ios_base::in) {
    return PROT_READ;
  } else if (mode == std::ios_base::out) {
    return PROT_WRITE;
  }
  return 0;
}

void fstream::refresh_file_status() {
  if (::fstat(fd_, &file_status_) == -1) {
    cerr << "Error getting file status " << strerror(errno) << endl;
    exit(errno);
  }
}

}  // namespace mem_map
