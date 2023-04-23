/**
 * Copyright 2023 - CSCE 311
 * Class Implementation for fstream
 * @author vnagpal
 */
#include "mmap_fstream.h"

#include <iostream>
using std::cout, std::cerr, std::clog, std::endl, mem_map::fstream;

fstream::fstream() {
  // default constructor only sets the mode
  mode_ = std::ios_base::in | std::ios_base::out;
}

fstream::fstream(const std::string& fname)
    : fstream(fname, std::ios_base::in | std::ios_base::out) {
  // calls main constructor with 2 params
}

fstream::fstream(const std::string& fname, std::ios_base::openmode mode) {
  // converts string to char*, sets mode
  file_name_ = strdup(fname.c_str());
  mode_ = mode;

  // calls open
  open(file_name_, mode_);
}

void fstream::open(const std::string& fname) {
  //returns if already open
  if (is_open()) return;
    // calls open with default mode
  open(fname, std::ios_base::in | std::ios_base::out);
}

void fstream::open(const std::string& fname, std::ios_base::openmode mode) {
  //returns if already open
  if (is_open()) return;

  // just in case
  file_name_ = strdup(fname.c_str());
  mode_ = mode;

  // if std::ios_base::ate was specified in mode, removes it using bitwise not
  // ~, also sets cursor end bool to true
  if ((cursor_end_ = (mode_ & std::ios_base::ate)))
    mode_ &= ~std::ios_base::ate;

  // based on mode sets o_flag_s and protection flags
  parse_mode();

  // opens the file descriptor
  fstream::open_fd();

  // calls ::fstat to get file status for ::stat struct file_status_
  refresh_file_status();

  // MEMORY MAP OPEN FILE, see mem_map() for ::mmap()
  fstream::mem_map();

  // if ios_mode::ate was specified, places cursor at the end of the file
  if (cursor_end_) cursor_ = size();

  // ALLOCATE FILE MEMORY, see fstream::truncate() for ::truncate()
  fstream::truncate(size());

  // indicates file is now open for reading and/or writing
  open_ = true;
}

void fstream::close() {
  if (!is_open()) return;

  // synchronizes memory map with actual file, so changes are reflected
  // SAVE TO DISK
  fstream::mem_sync();

  // closes file descriptor
  fstream::close_fd();

  // un-maps memory address
  fstream::mem_unmap();

  // file is no more open for reading and/or writing
  open_ = false;
}

bool fstream::is_open() const {  // returns open status of file
  return open_;
}

size_t fstream::size() const {  // returns size of file
  return file_status_.st_size;
}

fstream& fstream::put(char c) {
  // checks if file needs to be resized, and if so it truncates
  // ALLOCATE FILE MEMORY, see fstream::truncate() for ::truncate()
  if (size() == 0 || size() == cursor_) truncate(size() + 1);

  // places character at the cursor and increments cursor
  // WRITE TO FILE
  addr_[cursor_++] = c;

  // sync file after placing character
  // SAVE TO DISK
  fstream::mem_sync();

  // refreshes file status struct, so that size() will return correct updated
  // file size
  refresh_file_status();

  // to match with fstream&
  return *this;
}

char fstream::get() {
  // returns character at cursor, if at end of file returns null terminator
  // READ FROM FILE
  if (cursor_ < size()) return addr_[cursor_++];
  return '\0';
}

void fstream::parse_mode() {
  // set o_flag_, and protection flag based on given ios_base::open_mode

  // in - read, out - write, (in|out) - read and write
  if (mode_ == (std::ios_base::in | std::ios_base::out)) {
    o_flag_ |= O_RDWR;
    prot_flag_ |= (PROT_READ | PROT_WRITE);
  } else if (mode_ == std::ios_base::in) {
    o_flag_ |= O_RDONLY;
    prot_flag_ |= PROT_READ;
  } else if (mode_ == std::ios_base::out) {
    o_flag_ |= O_WRONLY;
    prot_flag_ |= PROT_WRITE;
  }

  // if file doesn't exist, adds an O_CREAT flag to create the file
  if (::access(file_name_, F_OK) != 0) o_flag_ |= O_CREAT;
}

void fstream::refresh_file_status() {
  // refreshes file status struct
  if (::fstat(fd_, &file_status_) == -1) {
    cerr << "Error getting file status " << strerror(errno) << endl;
    exit(errno);
  }
}

void fstream::open_fd() {
  // opens file and gets file descriptor
  fd_ = ::open(file_name_, o_flag_, 0664);
  if (fd_ < 0) {
    cerr << "couldn't open file, ::open() failed " << strerror(errno) << endl;
    exit(errno);
  }
}

void fstream::close_fd() {
  // closes file descriptor
  if (::close(fd_) < 0) {
    cerr << "couldn't close fd properly " << strerror(errno) << endl;
    exit(errno);
  }
}
void fstream::mem_map() {
  // memory maps address to program's memory
  addr_ = static_cast<char*>(
      ::mmap(nullptr, PAGE_SIZE, prot_flag_, MAP_SHARED, fd_, 0));
  if (addr_ == MAP_FAILED) {
    cerr << "couldn't mmap properly " << strerror(errno) << endl;
    exit(errno);
  }
}

void fstream::mem_unmap() {
  // unmapping memory address
  if (munmap(addr_, size()) == -1) {
    cerr << "Failed to unmap file " << strerror(errno) << endl;
    exit(errno);
  }
}

void fstream::mem_sync() {
  // synchronizes changes to reflect in file
  if (::msync(addr_, strlen(file_name_), MS_SYNC) < 0) {
    cerr << "msync error: " << strerror(errno) << endl;
    exit(errno);
  }
}

void fstream::truncate(size_t size) {
  // truncates file to size, but first checks writing permissions
  if ((::access(file_name_, W_OK) == 0) && (::truncate(file_name_, size) < 0)) {
    cerr << "::truncate error " << strerror(errno) << endl;
    exit(errno);
  }
}
