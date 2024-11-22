# Compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++17 -O2

# Directories
SRC_DIR = .
BUILD_DIR = build

# Source files
SOURCES = main.cpp \
          Pid.cpp \
		  OpenLoop.cpp \
          SetpointGenerator.cpp \
		  FirstOrderSystem.cpp \
		  SecondOrderSystem.cpp \
		  MassSpringDamper.cpp \
          Simulator.cpp

# Object files
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)

# Dependencies
DEPS = $(OBJECTS:.o=.d)

# Target executable
TARGET = main

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link the target
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET) data.txt

# Clean and rebuild
rebuild: clean all

# Run the program
run: all
	./$(TARGET)

# Print variables for debugging
debug:
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Dependencies: $(DEPS)"

.PHONY: all clean rebuild run debug