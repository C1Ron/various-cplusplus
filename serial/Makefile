# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -Og -g
LDFLAGS = -lboost_system -lboost_thread -lpthread

# Directories
OBJDIR = obj

# Source files
SRC = main.cpp \
		CommandHandler.cpp \
		SerialConnection.cpp \
		FrameBuilder.cpp \
		FrameInterpreter.cpp \
		SignalHandler.cpp \
		Logger.cpp

# Object files
OBJS = $(SRC:%.cpp=$(OBJDIR)/%.o)

# Executable name
EXE = main

# Default target
all: $(EXE)

# Linking the EXE
$(EXE): $(OBJS) | $(OBJDIR)
	$(CXX) $(OBJS) -o $(EXE) $(LDFLAGS)

# Compiling source files
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean target
clean:
	rm -rf $(OBJDIR) $(EXE) log.csv

# Phony targets
.PHONY: all clean

# Dependencies
$(OBJDIR)/main.o: main.cpp SerialConnection.h SignalHandler.h Logger.h
$(OBJDIR)/SerialConnection.o: SerialConnection.cpp SerialConnection.h
$(OBJDIR)/CommandHandler.o: CommandHandler.cpp CommandHandler.h SerialConnection.h \
		FrameBuilder.h FrameInterpreter.h Logger.h StMpcDefinitions.h
$(OBJDIR)/FrameBuilder.o: FrameBuilder.cpp FrameBuilder.h StMpcDefinitions.h
$(OBJDIR)/FrameInterpreter.o: FrameInterpreter.cpp FrameInterpreter.h StMpcDefinitions.h
$(OBJDIR)/SignalHandler.o: SignalHandler.cpp SignalHandler.h SerialConnection.h
$(OBJDIR)/Logger.o: Logger.cpp Logger.h SerialConnection.h StMpcDefinitions.h