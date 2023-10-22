/**
 * Copyright 2023 - CSCE 311
 * Class Declaration/Definition for fstream
 * @author vnagpal
 */
#include <errno.h>
#include <fcntl.h>     // O-Flags
#include <string.h>    // c-string manipulation
#include <sys/mman.h>  // memory mapping, protection flags
#include <sys/stat.h>  // ::fstat()
#include <unistd.h>    // access, close, truncate, etc.

#include <ios>     // openmodes
#include <string>  // string manipulation
#ifndef PROJECT4_MMAP_FSTREAM_H_
#define PROJECT4_MMAP_FSTREAM_H_
#define PAGE_SIZE 4096  // max file size

// mem_map namespace
namespace mem_map {

// fstream class
class fstream {
 public:
  /**
   * Constructor creates fstream object without file name, only sets default
   * open mode
   */
  fstream();

  /**
   * Constructor creates fstream object with file name and default open mode
   */
  explicit fstream(const std::string& fname);

  /**
   * Constructor creates fstream object with file name and specified open mode
   */
  fstream(const std::string& fname, std::ios_base::openmode mode);

  /**
   * Open method attemps to open a specified file with default open mode:
   * std::ios_base::in | std::ios_base::out
   * @param fname file path of specified file
   */
  void open(const std::string& fname);

  /**
   * Open method attemps to open a specified file with specified open mode
   * Can handle any combination of std::ios_base::ate, std::ios_base::in, and
   * std::ios_base::out
   * @param fname file path of specified file
   * @param mode open mode (std::ios_base:*)
   */
  void open(const std::string& fname, std::ios_base::openmode mode);

  /**
   * Close method closes a file, un-maps memory, and syncs to hard drive
   */
  void close();

  /**
   * returns file's open status
   * @return true if file is open, false otherwise
   */
  bool is_open() const;

  /**
   * returns size of the file
   * @return size of file (number of bytes being stored)
   */
  size_t size() const;

  /**
   * Gets the next character from a file specified by the cursor
   * @return next character from file, if cursor is at the end returns null
   * terminator
   */
  char get();

  /**
   * Writes a character at the next spot in the file and updates the cursor
   * @param c character being inserted into the file
   * @return fstream object (*this)
   */
  fstream& put(char c);

 private:
  // file descriptor
  int fd_;

  // file path
  char* file_name_;

  // memory mapped address
  char* addr_;

  // open mode of the file
  std::ios_base::openmode mode_;

  // file status, used for fstat and file size
  struct ::stat file_status_;

  // cursor, used to decided which spot the fstream will retrieve or insert a
  // character
  size_t cursor_ = 0;

  // used to determine whether the cursor should be at the end or not, default
  // false
  bool cursor_end_ = false;

  // used to determine whether the file is open for reading/writing, default
  // false
  bool open_ = false;

  // o_flag and prot_flag used for open() and mmap(), derived from mode_ (see
  // parse_mode)
  int o_flag_ = 0, prot_flag_ = 0;

  // parses the open mode into the proper o_flag and prot_flag
  void parse_mode();

  // Wrapper methods for c/linux system calls

  // calls ::fstat() to keep file size in ::stat updated
  void refresh_file_status();

  // calls ::open() to obtain file descriptor fd_
  void open_fd();

  // calls ::close() to close file_descriptor fd_
  void close_fd();

  // calls ::mmap() to obtain memory mapping of file
  void mem_map();

  // calls ::munmap() to free allocated memory used by ::mmap()
  void mem_unmap();

  // calls ::msync() to save changes made in memory map to hard disk
  void mem_sync();

  /**
   * calls ::truncate() to resize the file to the specified size
   * @param size
   */
  void truncate(size_t size);
};

}  // namespace mem_map
#endif  // PROJECT4_MMAP_FSTREAM_H_
