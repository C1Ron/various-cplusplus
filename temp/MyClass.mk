CXX = g++
CXXFLAGS = -Wall -g

# Source files and object files for the 'test' target
SRC_TEST = MyClass.cpp MyClassTest.cpp
OBJS_TEST = $(SRC_TEST:.cpp=.o)
EXEC_TEST = test

# Source files and object files for the 'server' target
SRC_SERVER = ServerTest.cpp
OBJS_SERVER = $(SRC_SERVER:.cpp=.o)
EXEC_SERVER = server

# Default target
all: test server

# Target for 'test'
test: $(EXEC_TEST)

$(EXEC_TEST): $(OBJS_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Target for 'server'
server: $(EXEC_SERVER)

$(EXEC_SERVER): $(OBJS_SERVER)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pattern rule for compiling .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Clean up generated files
clean:
	rm -f $(OBJS_TEST) $(EXEC_TEST) $(OBJS_SERVER) $(EXEC_SERVER)

.PHONY: all clean
