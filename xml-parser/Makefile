# Compiler and flags
CXX = g++
CXXFLAGS = `xml2-config --cflags --libs` -std=c++11
TARGETS = parser modernParser

# Default target: build both versions
all: $(TARGETS)

# Build parser version
parser: parser.cpp
	$(CXX) $(CXXFLAGS) -o parser parser.cpp

# Build modernParser version
modernParser: modernParser.cpp
	$(CXX) $(CXXFLAGS) -o modernParser modernParser.cpp

# Clean up the generated binaries
clean:
	rm -f $(TARGETS)
