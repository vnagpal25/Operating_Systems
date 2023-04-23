# CSCE311 Project 4 - Implementing File I/O using Memory Mapping in C++
Written by Vansh Nagpal

The purpose of this project was to implement a replica of the C/C++ fstream object using memory mapping.

(a) Where do you memory map the file?
  - I memory map the file within the the open() method of my fstream class.
  - See the call to fstream::mem_map() within the open method. 
  - Within my mem_map() method, I make the system call ::mmap().


(b) Where do you set the size of the file in the memory map using truncate()/ftruncate()?
  - I truncate my file in my open() method of my fstream class, where I make a call to fstream::truncate()
  - Within my fstream::truncate() method, I truncate the file if and only if the file has write permissions
  - I also make a call to fstream::truncate() within my put method, where I truncate the file size to increment by one if the cursor had reached the end of the file

(c) Where you read from the file contents using a memory map.
  - I read the contents of my from the memory map within my get() method of the fstream class, specifically where I index the memory address like a character array


(d) Where you write to the file using a memory map.
   - I write to the file using a memory map in the put(char) method.
   - Here I truncate the file if necessary and insert the character, once again indexing the address like a character array

(e) Where you save the file to disk using a memory map.
  - I save the file to disk using the memory map in my put() and close() methods.
  - I call fstream::mem_sync() in both of these methods, which makes a call to ::msync()

File Descriptions:
- mmap_fstream.h
  - class definition for fstream object using memory mapping

- mmap_fstream.cc
  - class implementation for fstream object using memory mapping

- test_mmap_fstream.cc
  - unit tests for mmap_fstream

- grow_mmap_file.cc
  - example file showing how to use memory mapping for file I/O