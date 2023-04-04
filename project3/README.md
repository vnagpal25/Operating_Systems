
# CSCE 311 Project 3 - IPC via Shared Memory and Synchronization
Written by Vansh Nagpal

The purpose of this project is to learn the use of Shared Memory, Semaphores, and Threads in a Linux environment

makefile: compiles c/c++ source/header files using c++ standard and warning flags
  - make: 
    - command in terminal compiles/links and creates an executable for the server and for the client
  - make csv-server:
    - command in terminal compiles/links and creates an executable for a server
  - make csv-client: 
    - command in terminal compiles/links and creates an executable for a client
  - make clean: 
    - command removes any *.o files and executables from the directory, leaving only source files

Parent Files:

  - inc/shm_parent.h
    - abstract class for client and server
    - contains all necessary headers for threading, shared memory, semaphores, and flags
    - defines constants that will be used by both client and server for shared memory and threading (BUF_SIZE, MAX_PATH_SIZE, THREAD_NUM, SHM_SIZE)
    - creates a struct that will be used to map shared memory segments on both client and server side
    - declares semaphore pointers that will be used to block client and server
    - declares shared memory file descriptor
    - contains a virtual Sleep() function that will block the process its called on and post the opposite process(i.e. client blocks itself and wakes up server and vice-versa)
    - see inc/shm_parent.h for more detailed documentation

Server Files:

  - src/main_server.cc:
    - main method entry point for server process
    - creates and runs an instance of the server class

  - src/shm_server.cc:
    - implements server class and all methods
    - RunServer() method runs the main process of the server, calling other helper methods
    - ProcessFile() method reads/writes to shared memory
    - server also creates and destroys named semaphores
    - see src/shm_server.cc for more detailed documentation

  - inc/shm_server.h:
    - defines server class
    - definitions for all methods, and helper methods
    - file path to search, and a shared memory struct pointer as class members
    - see inc/shm_server.h for more detailed documentation


Client Files:

  - src/main_client.cc:
    - main method entry point for client process
    - creates/runs instance of client, passes it the arguments entered from terminal

  - src/shm_client.cc:
    - implements client class and methods, as well as non class methods
    - has a mutex that will be used by searching threads to prevent race conditions
    - creates/destroys shared memory
    - connects to semaphores from server
    - copies over shared memory to local, and lets threads search those lines
    - prints out lines that match the arguments/operation criteria
    - also includes a function that inserts ANSI escape codes before each argument in a search string before printing them out, has the effect of them showing up as magenta in the linux terminal
    - see src/shm_client.cc for more detailed documentation

  - inc/shm_client.h:
    - defines client class and methods, as well as non class methods to be used by the main process
    - has shared memory struct, file path, and a struct that will be used by threads to search the shared memory
    - this struct for threads has a string for the line the thread will be searching, and a vector that will contain all of the lines that match the searching criteria
    - see inc/shm_client.h for more detailed documentation