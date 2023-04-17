// Copyright 2023 CSCE 311
//
// This file is a simple example of growing a file, page-by-page up to ten
// pages.
//
// Note that, in relation to your Project 4, you must determine exact file
// size(s), where here it simply grows to fill all pages.
//
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
//  Build with
//  g++ -std=c++17 -Wall -pedantic -o grow-mmap-file grow_mmap_file.cc
//


int main(int argc, char *argv[]) {
  char *addr;
  int fd;

  const int kTotalPages = 10;
  const int kPageSize = 4096;

  if (argc < 2) {
    printf("./grow-mmap-file file_name.txt\n");
    return 1;
  }

  fd = open(argv[1], O_RDWR | O_CREAT, 0664);
  if (fd < 0) {
    printf("open err: %s", strerror(errno));
    return 2;
  }

  addr = static_cast<char *>(mmap(NULL,
                             kTotalPages * kPageSize,
                             PROT_READ | PROT_WRITE,
                             MAP_SHARED,
                             fd,
                             0));
  if (addr == MAP_FAILED) {
    printf("mmap err: %s", strerror(errno));
    return 3;
  }

  if (close(fd) < 0) {       /* No longer need 'fd' */
    printf("close err: %s", strerror(errno));
    return 4;
  }

  if (truncate(argv[1], kPageSize) < 0) {
    printf("mmap err: %s", strerror(errno));
    return 5;
  }
  int pages_used = 1;

  for (int i = 0, ii = (int)'A'; i < kTotalPages * kPageSize; ++i, ++ii) {
    if (i >= pages_used * kPageSize) {
      ++pages_used;
      if (truncate(argv[1], pages_used * kPageSize) < 0) {
        printf("mmap err: %s", strerror(errno));
        return 6;
      }
    }
    
    addr[i] = (char)ii;
    if (ii == (int)'Z')
      ii = (int)'a' - 1;  // ++ in for-loop comes after
    if (ii == (int)'z')
      ii = (int)'A' - 1;  // ++ in for-loop comes after
  }

  int msync_sig = msync(addr, strlen(argv[1]), MS_SYNC);
  if (msync_sig < 0) {
    printf("msync err: %s", strerror(errno));
  }

  exit(EXIT_SUCCESS);
}
