# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17 -pthread
LDFLAGS = -ltbb

# Target executable
TARGET = tema1

# Source files
SRCS = main.cpp map.cpp reduce.cpp functions.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Rule for compiling source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
	rm -r output

# Build alias for checker script
build: all
