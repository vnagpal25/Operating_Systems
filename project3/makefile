cc = g++  # use g++ compiler
path = ..
standard = c++17

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -pedantic  # even more warnings
flags += -I $(path)  # add parent to preprocessor include path
flags += -g  # include gdb instrumentation
flags += -pthread # include threads/semaphore support

link = $(cc) $(flags) $^ -o #links all prerequisites

compile = $(cc) $(flags) -c $< -o #compiles first prerequisite

make: bin csv-client csv-server
	
csv-client: bin/main_client.o bin/shm_client.o
	$(link) $@ -lrt
	
csv-server: bin/main_server.o bin/shm_server.o
	$(link) $@ -lrt

bin:
	mkdir -p bin

bin/main_server.o: src/main_server.cc inc/shm_parent.h inc/shm_server.h | bin
	$(compile) $@

bin/shm_server.o: src/shm_server.cc inc/shm_parent.h inc/shm_server.h | bin
	$(compile) $@

bin/main_client.o: src/main_client.cc inc/shm_client.h | bin
	$(compile) $@

bin/shm_client.o: src/shm_client.cc inc/shm_parent.h inc/shm_client.h | bin
	$(compile) $@

clean:
	rm -f bin/*.o csv-client csv-server
