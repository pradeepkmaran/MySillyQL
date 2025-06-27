CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = sillyql
TEST_TARGET = test_sillyql

SRCDIR = src
TESTDIR = tests
OBJDIR = build

SOURCES = $(shell find $(SRCDIR) -name "*.cpp")
TEST_SOURCES = $(shell find $(SRCDIR) -name "*.cpp" | grep -v main.cpp) $(TESTDIR)/test_main.cpp
OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

test: $(TEST_TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CXX) $(filter-out $(OBJDIR)/src/main.o, $(TEST_OBJECTS)) -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET) $(TEST_TARGET)

.PHONY: all test clean