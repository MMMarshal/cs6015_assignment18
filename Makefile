main:	clean
	clang++ -std=c++17 -O3 -DNDEBUG main.cpp -o simulator
clean:
	rm -f simulator

format:
	clang-format -style=file main.cpp -i

