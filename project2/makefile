cc = g++  # use g++ compiler
path = ..
standard = c++17

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -pedantic  # even more warnings
flags += -I $(path)  # add parent to preprocessor include path
flags += -g  # include gdb instrumentation

link = $(cc) $(flags) $^ -o  # final linked build to binary executable

compile = $(cc) $(flags) -c $< -o  # compilation to intermediary .o files

make: text-client text-server
	
text-client:bin/domain_socket.o bin/domain_socket_client.o bin/main_client.o
	$(link) $@
	
text-server:bin/domain_socket.o bin/domain_socket_server.o bin/main_server.o
	$(link) $@

bin:
	mkdir -p bin

bin/main_client.o: src/main_client.cc inc/domain_socket.h inc/domain_socket_client.h | bin
	$(compile) $@

bin/main_server.o: src/main_server.cc inc/domain_socket.h inc/domain_socket_server.h | bin
	$(compile) $@

bin/domain_socket_client.o: src/domain_socket_client.cc inc/domain_socket_client.h inc/domain_socket.h | bin
	$(compile) 	$@

bin/domain_socket_server.o: src/domain_socket_server.cc inc/domain_socket_server.h inc/domain_socket.h | bin
	$(compile) $@

bin/domain_socket.o: 	src/domain_socket.cc inc/domain_socket.h
	$(compile) $@

clean:
	rm -f bin/*.o text-client text-server
