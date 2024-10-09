# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS = -lboost_system -lboost_thread -lpthread

# Directories
OBJDIR = obj

# Source files
SOURCES = main.cpp \
          SerialConnection.cpp \
          CommandHandler.cpp \
          CommandLine.cpp \
          FastLogger.cpp \
          FrameBuilder.cpp \
          FrameInterpreter.cpp \
          SignalHandler.cpp

# Object files
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# Executable name
EXECUTABLE = main

# Default target
all: $(EXECUTABLE)

# Linking the executable
$(EXECUTABLE): $(OBJECTS) | $(OBJDIR)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

# Compiling source files
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean target
clean:
	rm -rf $(OBJDIR) $(EXECUTABLE)

# Phony targets
.PHONY: all clean

# Dependencies
$(OBJDIR)/main.o: main.cpp SerialConnection.h CommandLine.h SignalHandler.h StMpcDefinitions.h
$(OBJDIR)/SerialConnection.o: SerialConnection.cpp SerialConnection.h StMpcDefinitions.h
$(OBJDIR)/CommandHandler.o: CommandHandler.cpp CommandHandler.h SerialConnection.h FrameBuilder.h FrameInterpreter.h StMpcDefinitions.h
$(OBJDIR)/CommandLine.o: CommandLine.cpp CommandLine.h CommandHandler.h StMpcDefinitions.h
$(OBJDIR)/FastLogger.o: FastLogger.cpp FastLogger.h SerialConnection.h StMpcDefinitions.h
$(OBJDIR)/FrameBuilder.o: FrameBuilder.cpp FrameBuilder.h StMpcDefinitions.h
$(OBJDIR)/FrameInterpreter.o: FrameInterpreter.cpp FrameInterpreter.h StMpcDefinitions.h
$(OBJDIR)/SignalHandler.o: SignalHandler.cpp SignalHandler.h SerialConnection.h