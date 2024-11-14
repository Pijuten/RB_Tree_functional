# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic

# Source files
SRC = main.cpp

# Output executable
OUT = main

# Doctest-related
DOCTEST_FLAG = -DDOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

# Build with doctest
build_with_doctest: CXXFLAGS += $(DOCTEST_FLAG)
build_with_doctest: $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC)

# Build without doctest
build_without_doctest: $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC)

# Clean
clean:
	rm -f $(OUT)