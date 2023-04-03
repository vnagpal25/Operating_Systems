cc = g++  # use g++ compiler
path = ..
standard = c++17

flags = -std=$(standard)  # compile with standard
flags += -Wall  # compile with all warnings
flags += -pedantic  # even more warnings
flags += -I $(path)  # add parent to preprocessor include path
flags += -g  # include gdb instrumentation

link = $(cc) $(flags) -o  # final linked build to binary executable

compile = $(cc) $(flags) -c  # compilation to intermediary .o files

make: text-client text-server
	
text-client:domain_socket.o domain_socket_client.o main_client.o
	$(cc) $(flags) $^ -o $@
	
text-server:domain_socket.o domain_socket_server.o main_server.o
	$(cc) $(flags) $^ -o $@

main_client.o: main_client.cc domain_socket.h domain_socket_client.h
	$(compile) main_client.cc

main_server.o: main_server.cc domain_socket.h domain_socket_server.h
	$(compile) main_server.cc

domain_socket_client.o: domain_socket_client.cc domain_socket_client.h domain_socket.h
	$(compile) 	domain_socket_client.cc

domain_socket_server.o: domain_socket_server.cc domain_socket_server.h domain_socket.h
	$(compile) domain_socket_server.cc

domain_socket.o: 	domain_socket.cc domain_socket.h
	$(compile) domain_socket.cc

clean:
	rm *.o text-client text-server
