# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -Og -g
LDFLAGS = -lboost_system -lboost_thread -lpthread -lreadline

# Directories
OBJDIR = obj

# Source files
SRC = mainRtIf.cpp \
      CommandHandlerRt.cpp \
      SerialConnection.cpp \
      FrameBuilderRt.cpp \
      FrameInterpreterRt.cpp \
      SignalHandler.cpp \
      LoggerRt.cpp \
      RtInterface.cpp

# Object files
OBJS = $(SRC:%.cpp=$(OBJDIR)/%.o)

# Executable name
EXE = mainRtIf

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
	rm -rf $(OBJDIR) $(EXE) *log*.csv

# Phony targets
.PHONY: all clean

# Dependencies
$(OBJDIR)/mainRtIf.o: mainRtIf.cpp RtInterface.h
$(OBJDIR)/RtInterface.o: RtInterface.cpp RtInterface.h SerialConnection.h SignalHandler.h LoggerRt.h CommandHandlerRt.h
$(OBJDIR)/SerialConnection.o: SerialConnection.cpp SerialConnection.h
$(OBJDIR)/CommandHandlerRt.o: CommandHandlerRt.cpp CommandHandlerRt.h SerialConnection.h \
        FrameBuilderRt.h FrameInterpreterRt.h LoggerRt.h RtDefinitions.h
$(OBJDIR)/FrameBuilderRt.o: FrameBuilderRt.cpp FrameBuilderRt.h RtDefinitions.h
$(OBJDIR)/FrameInterpreterRt.o: FrameInterpreterRt.cpp FrameInterpreterRt.h RtDefinitions.h
$(OBJDIR)/SignalHandler.o: SignalHandler.cpp SignalHandler.h
$(OBJDIR)/LoggerRt.o: LoggerRt.cpp LoggerRt.h SerialConnection.h RtDefinitions.h