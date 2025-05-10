CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O3 -march=native
LDFLAGS = 

# Output binary name
TARGET = external_sort

# Source files
SRCS = main.cpp external_sort.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target (build everything)
all: $(TARGET)

# Link object files to create the target executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target (remove executable and object files)
clean:
	rm -f $(TARGET) $(OBJS)

# Clean everything, including temporary data files
clean-all: clean
	rm -f *.bin results.csv

# Rebuild everything from scratch
rebuild: clean all

# Find optimal arity value
find-arity: all
	./$(TARGET) find_arity

# Run tests with specified arity (default 4)
run-tests: all
	./$(TARGET) run_tests 4

# Run full experiment (find optimal arity and run tests)
full: all
	./$(TARGET) full

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build the executable"
	@echo "  clean      - Remove object files and executable"
	@echo "  clean-all  - Remove object files, executable, and temporary data files"
	@echo "  rebuild    - Rebuild everything from scratch"
	@echo "  find-arity - Find the optimal arity value only"
	@echo "  run-tests  - Run experiments with default arity (4)"
	@echo "  full       - Find optimal arity and run all experiments"
	@echo "  help       - Show this help message"

.PHONY: all clean clean-all rebuild find-arity run-tests full help