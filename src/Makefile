# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# Targets and dependencies
TARGET = algo.exe
OBJECTS = main.o Trader.o Api.o Socket.o BSocket.o Socketpp.o

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) -lssl -lcrypto -lboost_system

# Compile main.cpp to main.o
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

# Compile Trader.cpp to Trader.o
Trader.o: Trader.cpp
	$(CXX) $(CXXFLAGS) -c Trader.cpp -o Trader.o

# Compile Api.cpp to Api.o
Api.o: Api.cpp
	$(CXX) $(CXXFLAGS) -c Api.cpp -o Api.o

# Socket Dependencies
Socket.o: Socket.cpp
	$(CXX) $(CXXFLAGS) -c Socket.cpp -o Socket.o

# BSocket Dependencies
BSocket.o: BSocket.cpp
	$(CXX) $(CXXFLAGS) -c BSocket.cpp -o BSocket.o

# Socketpp Dependencies
Socketpp.o: Socketpp.cpp
	$(CXX) $(CXXFLAGS) -c Socketpp.cpp -o Socketpp.o



# tests
# test_latency
test_latency.o: test_latency.cpp
	$(CXX) $(CXXFLAGS) -c test_latency.cpp -o test_latency.o


# Clean up generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean

