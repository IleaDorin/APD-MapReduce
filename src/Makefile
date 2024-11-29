CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17 -pthread

SRCS = main.cpp map.cpp reduce.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = tema

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
