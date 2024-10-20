# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS = -lboost_system -lboost_thread -lpthread

# Directories
OBJDIR = obj

# Source files
SRC1 = main.cpp \
		  SerialConnection.cpp \
		  CommandHandler.cpp \
		  CommandLine.cpp \
		  FastLogger.cpp \
		  TurboLogger.cpp \
		  FrameBuilder.cpp \
		  FrameInterpreter.cpp \
		  SignalHandler.cpp
SRC2 = VirtualMsc.cpp VirtualMscMain.cpp

# Object files
OBJS1 = $(SRC1:%.cpp=$(OBJDIR)/%.o)
OBJS2 = $(SRC2:%.cpp=$(OBJDIR)/%.o)

# Executable name
EXE1 = main
EXE2 = virtualMsc

# Default target
all: $(EXE1) $(EXE2)

# Linking the EXE1
$(EXE1): $(OBJS1) | $(OBJDIR)
	$(CXX) $(OBJS1) -o $(EXE1) $(LDFLAGS)

# Linking to EXE2
$(EXE2): $(OBJS2) | $(OBJDIR)
	$(CXX) $(OBJS2) -o $(EXE2)

# Compiling source files
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean target
clean:
	rm -rf $(OBJDIR) $(EXE1) $(EXE2) log.csv

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
$(OBJDIR)/VirtualMsc.o: VirtualMscMain.cpp VirtualMsc.cpp VirtualMsc.h