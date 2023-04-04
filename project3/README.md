
# CSCE 311 Project 3 - IPC via Shared Memory and Synchronization
Written by Vansh Nagpal

The purpose of this project is to learn the use of Shared Memory, Semaphores, and Threads in a Linux environment

makefile: compiles c/c++ source/header files using c++ standard and warning flags
  - make: command in terminal compiles/links and creates an executable for the server and for the client
  - make csv-server: command in terminal compiles/links and creates an executable for a server
  - make csv-client: command in terminal compiles/links and creates an executable for a client
  - make clean: command removes any *.o files and executables from the directory, leaving only source files

Parent Files:
  - inc/shm_parent.h
    - abstract class

Server Socket Files:

  - src/main_server.cc:

  - src/shm_server.cc:

  - inc/shm_server.h


Client Socket Files:

  - src/main_client.cc:

  - src/shm_client.cc:

  - inc/shm_client.h: