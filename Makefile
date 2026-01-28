# Makefile for JUST (C project)

# Compiler and flags
CC := clang
CFLAGS := -W -Wall -Wextra -Werror -std=c11 -Iinclude
LDFLAGS :=

# Build modes
DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

# Directories
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include

# Target
TARGET := $(BIN_DIR)/just

# Sources and objects
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Tools
CLANG_FORMAT := clang-format
CPPCHECK := cppcheck
CLANG_TIDY := clang-tidy

# Colors for output
BLUE := \033[0;34m
GREEN := \033[0;32m
RED := \033[0;31m
RESET := \033[0m

# Default target
.DEFAULT_GOAL := all

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	@printf "$(GREEN)Linking$(RESET) $@\n"
	@$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@printf "$(BLUE)Compiling$(RESET) $<\n"
	@$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# Clean targets
clean:
	@printf "$(RED)Cleaning$(RESET) build artifacts\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "$(RED)Full clean$(RESET) complete\n"
	@rm -rf $(BIN_DIR)

re: fclean all

# Code quality targets
format:
	@printf "$(BLUE)Formatting$(RESET) code with clang-format\n"
	@$(CLANG_FORMAT) -i $(SRCS) $(wildcard $(INCLUDE_DIR)/*.h)

check:
	@printf "$(BLUE)Running$(RESET) cppcheck\n"
	@$(CPPCHECK) $$(grep -v '^\#' .cppcheck.cfg | grep '^--' | tr '\n' ' ') $(SRCS)
	@printf "$(BLUE)Running$(RESET) clang-tidy\n"
	@$(CLANG_TIDY) --config-file=.clang-tidy $(SRCS) -- $(CFLAGS)

# Run the program
run: $(TARGET)
	@printf "$(GREEN)Running$(RESET) $(TARGET)\n"
	@./$(TARGET)

# Debug build
debug:
	@$(MAKE) DEBUG=1

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  fclean  - Full clean"
	@echo "  re      - Rebuild from scratch"
	@echo "  format  - Format code with clang-format"
	@echo "  check   - Run static analysis (cppcheck + clang-tidy)"
	@echo "  run     - Build and run the program"
	@echo "  debug   - Build with debug symbols"
	@echo "  help    - Show this help message"

# Phony targets
.PHONY: all clean fclean re format check run debug help

