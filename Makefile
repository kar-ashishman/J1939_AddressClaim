CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET := main
SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@



clean:
	rm -f $(TARGET) $(OBJ)
