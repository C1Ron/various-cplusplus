CXXFLAGS = -std=c++23 -pthread

CXXBench = -std=c++23 -O3 -I../../benchmark/include
CXXLibBench = -L../../benchmark/build/src -lbenchmark -lpthread

dateTest: dateTest.cpp
	g++ $(CXXFLAGS) -o dateTest dateTest.cpp DateClass.cpp DateClass2.cpp

bench: bench.cpp DateClass.cpp DateClass2.cpp
	g++ $(CXXBench) bench.cpp DateClass.cpp DateClass2.cpp  -o bench $(CXXLibBench)

bench2: bench2.cpp DateClass.cpp DateClass2.cpp
	g++ $(CXXBench) bench2.cpp DateClass.cpp DateClass2.cpp  -o bench2 $(CXXLibBench)


clean:
	rm -f dateTest bench bench2