CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -g

SOURCES = main.cpp
TEST_SOURCES = tests/test_skip_list.cpp
HEADERS = skip_list.hpp

TARGET = skip_list_demo
TEST_TARGET = skip_list_tests

all: $(TARGET) $(TEST_TARGET)


$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)


$(TEST_TARGET): $(TEST_SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o $(TEST_TARGET) -lgtest -lgtest_main -pthread


test: $(TEST_TARGET)
	./$(TEST_TARGET)


docs: Doxyfile
	doxygen Doxyfile

clean:
	rm -f $(TARGET) $(TEST_TARGET)
	rm -rf docs

ci: clean all test
	@echo "CI/CD Pipeline completed successfully!"

.PHONY: all test docs clean ci
