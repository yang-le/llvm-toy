all: toy

clean:
	rm -rf *.dwo *.o toy

toy: toy.o scanner.o ast.o parser.o common.o
	clang++ $^ `llvm-config --ldflags --system-libs --libs core` -o $@

%.o : %.cpp
	clang++ `llvm-config --cxxflags` -c $^ -o $@
