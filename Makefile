# Add variables like this
# or with an equals sign (I think)
FLAGS := -std=c++17 -I./include

# Run the program and compile it if it needs to be
# (make / make run)
run: adk.o
	./adk.exe tests/output.adk

# Compile the program (make build)
build:
adk.o: src/main.cpp include/lexer.hpp
	g++ $(FLAGS) -c src/main.cpp -o adk.o
	g++ $(FLAGS) adk.o -o adk.exe

# Clean any executable and object files
clean:
	rm *.exe *.o