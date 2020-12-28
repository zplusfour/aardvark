# Add variables like this
# or with an equals sign (I think)
FLAGS := -std=c++17 -I./include
FILES := src/main.cpp include/lexer.hpp include/parser.hpp include/interpreter.hpp

# Run the program and compile it if it needs to be
# (make / make run)
run: adk.o
	./bin/adk.exe tests/output.adk

# Compile the program (make build)
build:
adk.o: $(FILES)
	g++ $(FLAGS) -c src/main.cpp -o adk.o
	g++ $(FLAGS) adk.o -o bin/adk.exe

# Clean any executable and object files
clean:
	rm *.exe *.o