CXX = g++
CXXFLAGS = -Wall -Werror -std=c++17 -pthread
LDFLAGS = -ltbb

all: tema

tema: main.o map.o reduce.o functions.o
	$(CXX) $(CXXFLAGS) -o tema main.o map.o reduce.o functions.o $(LDFLAGS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

map.o: map.cpp
	$(CXX) $(CXXFLAGS) -c map.cpp

reduce.o: reduce.cpp
	$(CXX) $(CXXFLAGS) -c reduce.cpp

functions.o: functions.cpp
	$(CXX) $(CXXFLAGS) -c functions.cpp

clean:
	rm -f *.o tema
