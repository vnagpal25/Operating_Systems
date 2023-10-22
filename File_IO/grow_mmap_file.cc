// Copyright 2023 CSCE 311
//
// This file is a simple example of growing a file, page-by-page up to ten
// pages.
//
// Note that, in relation to your Project 4, you must determine exact file
// size(s), where here it simply grows to fill all pages.
//
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

//
//  Build with
//  g++ -std=c++17 -Wall -pedantic -o grow-mmap-file grow_mmap_file.cc
//

int main(int argc, char *argv[]) {
  char *addr;  // address of shared memory
  int fd;      // file descriptor

  const int kTotalPages = 10;
  const int kPageSize = 4096;

  if (argc < 2) {
    printf("./grow-mmap-file file_name.txt\n");
    return 1;
  }
  // opens file given by name provided by user, creates it if
  // it doesn't exist
  fd = open(argv[1], O_RDWR | O_CREAT, 0664);

  // fd should be greater than 0, if not there was an error
  if (fd < 0) {
    printf("open err: %s", strerror(errno));
    return 2;
  }

  // maps memory address to addr
  addr = static_cast<char *>(mmap(NULL, kTotalPages * kPageSize,
                                  PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

  // error checking
  if (addr == MAP_FAILED) {
    printf("mmap err: %s", strerror(errno));
    return 3;
  }

  // closes file descriptor since we already have mapping to it specified by
  // addr

  if (close(fd) < 0) { /* No longer need 'fd' */
    printf("close err: %s", strerror(errno));
    return 4;
  }

  // basically requests for file to have 4096 bytes able to be written to
  if (truncate(argv[1], kPageSize) < 0) {
    printf("mmap err: %s", strerror(errno));
    return 5;
  }
  int pages_used = 1;

  for (int i = 0, ii = (int)'A'; i < kTotalPages * kPageSize; ++i, ++ii) {
    if (i >= pages_used * kPageSize) {
      ++pages_used;
      // extends file size to another page in memory
      if (truncate(argv[1], pages_used * kPageSize) < 0) {
        printf("mmap err: %s", strerror(errno));
        return 6;
      }
    }

    addr[i] = (char)ii;
    if (ii == (int)'Z') ii = (int)'a' - 1;  // ++ in for-loop comes after
    if (ii == (int)'z') ii = (int)'A' - 1;  // ++ in for-loop comes after
  }

  // msync synchronizes the memory map we have specified in addr, with the
  // actual file after this, changes we have made to the memory map will be
  // reflected in the file itself in the directory
  int msync_sig = msync(addr, strlen(argv[1]), MS_SYNC);
  if (msync_sig < 0) {
    printf("msync err: %s", strerror(errno));
  }

  exit(EXIT_SUCCESS);
}
