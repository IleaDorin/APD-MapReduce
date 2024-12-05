# compiler and flags
CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17 -pthread

# target executable
TARGET = tema1

# sf
SRCS = main.cpp map.cpp reduce.cpp functions.cpp concurrent_map.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

build: all
