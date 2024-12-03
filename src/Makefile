# compiler and flags
CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17 -pthread
LDFLAGS = -ltbb

# target executable
TARGET = tema1

# source files
SRCS = main.cpp map.cpp reduce.cpp functions.cpp
OBJS = $(SRCS:.cpp=.o)

# default target
all: $(TARGET)

# build the main executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -r output

build: all
