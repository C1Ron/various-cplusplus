# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

# Target Executables
TARGET = uart-com
TARGET2 = uart-com-sim
TARGET3 = uart-cli

# Source Files for each target
SRCS = UartCom.cpp UartApplication.cpp uart-com.cpp
SRCS2 = UartCom.cpp UartApplication.cpp VirtualUart.cpp uart-com-sim.cpp
SRCS3 = UartCom.cpp UartApplication.cpp VirtualUart.cpp main.cpp

# Object Files (generated automatically from SRCS)
OBJS = $(SRCS:.cpp=.o)
OBJS2 = $(SRCS2:.cpp=.o)
OBJS3 = $(SRCS3:.cpp=.o)

# Header Files for dependency checking
HDRS = UartComIF.h UartCom.h UartApplication.h 
HDRS2 = UartComIF.h UartCom.h UartApplication.h VirtualUart.h
HDRS3 = UartComIF.h UartCom.h UartApplication.h VirtualUart.h

# Build all three targets
all: $(TARGET) $(TARGET2) $(TARGET3)

# Link the object files to create the final executables
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(TARGET2): $(OBJS2)
	$(CXX) $(CXXFLAGS) -o $(TARGET2) $(OBJS2)

$(TARGET3): $(OBJS3)
	$(CXX) $(CXXFLAGS) -o $(TARGET3) $(OBJS3)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean the build directory
clean:
	rm -f $(OBJS) $(TARGET) $(OBJS2) $(TARGET2) $(OBJS3) $(TARGET3) *.txt

# Phony targets so that make doesn't get confused with filenames
.PHONY: all clean
