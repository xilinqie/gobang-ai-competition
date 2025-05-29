# Makefile for Gobang AI Competition Program

# Compiler and flags
CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c99
TARGET = gobang_ai
SOURCE = gobang_ai.c

# Default target
all: $(TARGET)

# Build the main program
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Clean up generated files
clean:
	rm -f $(TARGET)

# Run tests (if any test files exist)
test: $(TARGET)
	@echo "Running basic functionality test..."
	@echo "START 1" | ./$(TARGET) | head -1

# Install (copy to system path - optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Debug build with additional flags
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build with maximum optimization
release: CFLAGS += -O3 -DNDEBUG
release: $(TARGET)

# Check code formatting and style
check:
	@echo "Checking code style..."
	@cppcheck --enable=all $(SOURCE) 2>/dev/null || echo "cppcheck not available"

# Show program info
info:
	@echo "Gobang AI Competition Program"
	@echo "Source: $(SOURCE)"
	@echo "Target: $(TARGET)"
	@echo "Compiler: $(CC)"
	@echo "Flags: $(CFLAGS)"

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build the program (default)"
	@echo "  clean    - Remove generated files"
	@echo "  test     - Run basic functionality test"
	@echo "  debug    - Build with debug information"
	@echo "  release  - Build with maximum optimization"
	@echo "  check    - Check code style"
	@echo "  info     - Show build information"
	@echo "  help     - Show this help message"

# Declare phony targets
.PHONY: all clean test install uninstall debug release check info help