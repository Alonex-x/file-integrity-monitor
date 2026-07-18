CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lssl -lcrypto

TARGET = file-integrity-monitor
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/sha256.cpp $(SRC_DIR)/integrity.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean

test: run_tests
	./run_tests

run_tests: tests/test_sha256.cpp tests/test_integrity.cpp src/sha256.cpp src/integrity.cpp
	g++ -std=c++17 -Wall -Wextra -Isrc -c tests/test_sha256.cpp -o test_sha256.o
	g++ -std=c++17 -Wall -Wextra -Isrc -c tests/test_integrity.cpp -o test_integrity.o
	g++ -std=c++17 -Wall -Wextra -Isrc -c src/sha256.cpp -o sha256_lib.o
	g++ -std=c++17 -Wall -Wextra -Isrc -c src/integrity.cpp -o integrity_lib.o
	g++ test_sha256.o test_integrity.o sha256_lib.o integrity_lib.o \
	    -lgtest -lgtest_main -pthread -lssl -lcrypto \
	    -o run_tests

.PHONY: test
