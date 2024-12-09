# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -Og -g
LDFLAGS = -lboost_system -lboost_thread -lpthread -lreadline

# Directories
OBJDIR = obj

# Source files
SRC1 = main.cpp \
		CommandHandler.cpp \
		SerialConnection.cpp \
		FrameBuilder.cpp \
		FrameInterpreter.cpp \
		SignalHandler.cpp \
		Logger.cpp 

SRC2 = mainMscIf.cpp \
		CommandHandler.cpp \
		SerialConnection.cpp \
		FrameBuilder.cpp \
		FrameInterpreter.cpp \
		SignalHandler.cpp \
		Logger.cpp \
		MscInterface.cpp

# Object files
OBJS1 = $(SRC1:%.cpp=$(OBJDIR)/%.o)
OBJS2 = $(SRC2:%.cpp=$(OBJDIR)/%.o)

# Executable name
EXE1 = main
EXE2 = mscIf

# Default target
all: $(EXE1) $(EXE2)

# Linking the EXE
$(EXE1): $(OBJS1) | $(OBJDIR)
	$(CXX) $(OBJS1) -o $(EXE1) $(LDFLAGS)

$(EXE2): $(OBJS2) | $(OBJDIR)
	$(CXX) $(OBJS2) -o $(EXE2) $(LDFLAGS)

# Compiling source files
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean target
clean:
	rm -rf $(OBJDIR) $(EXE1) $(EXE2) *log*.csv

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
$(OBJDIR)/mainMscIf.o: mainMscIf.cpp SerialConnection.h SignalHandler.h Logger.h MscInterface.h