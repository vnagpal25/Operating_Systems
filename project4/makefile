# Copyright 2023 CSCE 311

cc = g++  # use g++ compiler
path = .
standard = c++17

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -pedantic  # even more warnings
flags += -I $(path)  # add parent to preprocessor include path
flags += -g  # include gdb instrumentation

link = $(cc) $(linker_flags) -o  # final linked build to binary executable

compile = $(cc) $(flags) -c -o  # compilation to intermediary .o files

# tests
#
test-all: test-open test-get test-put-new test-put-ate
	@echo 'ALL TESTS RAN'

test-open: test-mmap-fstream
	rm -f test_open.txt
	echo -n "0123456789" > test_open.txt
	./test-mmap-fstream 0

test-get: test-mmap-fstream
	rm -f test_get.txt
	echo -n "wxyz" > test_get.txt
	chmod 444 test_get.txt
	./test-mmap-fstream 1

test-put-new: test-mmap-fstream
	rm -f test_put.txt
	./test-mmap-fstream 2

test-put-ate: test-mmap-fstream
	echo -n "abcd" > test_ate.txt
	./test-mmap-fstream 3

find-comments:
	@grep -wns "MEMORY MAP OPEN FILE" mmap_fstream.cc -A 2
	@grep -wns "ALLOCATE FILE MEMORY" mmap_fstream.cc -A 2
	@grep -wns "READ FROM FILE" mmap_fstream.cc -A 2
	@grep -wns "SAVE TO DISK" mmap_fstream.cc -A 2

# executable
#
test-mmap-fstream: test_mmap_fstream.o mmap_fstream.o
	$(link) $@ $^

# objects
#
test_mmap_fstream.o : test_mmap_fstream.cc mmap_fstream.h
	$(compile) $@ $<
#
bin/mmap_fstream.o : src/mmap_fstream.cc inc/mmap_fstream.h
	$(compile) $@ $<


# utils
#
clean:
	rm -f bin/*.o test-mmap-fstream \
  test_ate.txt test_get.txt test_open.txt test_put.txt
