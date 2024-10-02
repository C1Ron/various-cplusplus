# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -I$(BOOST_INCLUDE)

# Boost library paths
BOOST_LIBS = -lboost_system
BOOST_LIB_PATH = ~/downloads/boost_1_85_0/stage/lib
BOOST_INCLUDE = ~/downloads/boost_1_85_0

# Output binary
TARGET = main

# Source files
SRCS = main.cpp SerialConnection.cpp FrameBuilder.cpp FrameInterpreter.cpp CommandHandler.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -L$(BOOST_LIB_PATH) $(BOOST_LIBS)

# Compile individual source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(TARGET) $(OBJS)

# Clean object files
clean-obj:
	rm -f $(OBJS)