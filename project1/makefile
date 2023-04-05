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

make: calculate
	
calculate:bin/main.o bin/calculator.o
	$(link) $@
	
bin:
	mkdir -p bin

bin/main.o: src/main.cc inc/calculator.h | bin
	$(compile) 	$@

bin/calculator.o: src/calculator.cc inc/calculator.h | bin
	$(compile) $@

clean:
	rm -f bin/*.o calculate
