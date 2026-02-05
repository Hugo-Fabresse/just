# Makefile for JUST (C project)

# Compiler and flags
CC := clang
CFLAGS := -W -Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L -Iinclude
LDFLAGS :=
COV_FLAGS := -fprofile-instr-generate -fcoverage-mapping

# Build modes
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g -O0 -DDEBUG
else
	CFLAGS += -O2 -DNDEBUG
endif

# Directories
SRC_DIR := src
TEST_DIR := tests
OBJ_DIR := build
BIN_DIR := bin

# Targets
TARGET := $(BIN_DIR)/just
TEST_BIN := $(BIN_DIR)/tests

# Sources
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))
TEST_SRCS := $(shell find $(TEST_DIR) -name '*.c')
TEST_OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(TEST_SRCS))

# Colors
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

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@printf "$(BLUE)Compiling$(RESET) $<\n"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	@mkdir -p $@

# Cleaning
clean:
	@printf "$(RED)Cleaning$(RESET) objects\n"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@printf "$(RED)Full clean$(RESET)\n"
	@rm -rf $(BIN_DIR) *.profraw *.profdata

re: fclean all

# Tests
test: CFLAGS += -g
test: fclean $(TEST_BIN)
	@printf "$(GREEN)Running$(RESET) tests\n"
	@./$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS) $(filter-out $(OBJ_DIR)/$(SRC_DIR)/main.o,$(OBJS)) | $(BIN_DIR)
	@printf "$(GREEN)Linking$(RESET) tests\n"
	@$(CC) $^ -o $@ -lcriterion

# Coverage (LLVM + Criterion)
coverage: CFLAGS += $(COV_FLAGS)
coverage: LDFLAGS += $(COV_FLAGS)
coverage: fclean
	@printf "$(GREEN)Compiling tests and source for coverage$(RESET)\n"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(filter-out $(SRC_DIR)/main.c,$(SRCS)) $(TEST_SRCS) -o $(TEST_BIN) -lcriterion
	@printf "$(GREEN)Running tests with coverage$(RESET)\n"
	@rm -f *.profraw *.profdata
	@LLVM_PROFILE_FILE="coverage.profraw" ./$(TEST_BIN) --jobs=1 || true
	@printf "$(GREEN)Generating coverage report$(RESET)\n"
	@if [ -f coverage.profraw ]; then \
		llvm-profdata merge -sparse coverage.profraw -o coverage.profdata 2>/dev/null || \
		(rm -f coverage.profraw coverage.profdata && echo "$(RED)Failed to merge coverage data$(RESET)" && exit 1); \
	fi
	@if [ -f coverage.profdata ]; then \
		llvm-cov report ./$(TEST_BIN) \
			-instr-profile=coverage.profdata \
			-ignore-filename-regex="$(TEST_DIR)"; \
	else \
		echo "$(RED)No coverage data generated$(RESET)"; \
		exit 1; \
	fi

# Utilities
format:
	@printf "$(BLUE)Formatting$(RESET) code\n"
	@clang-format -i $(SRCS) $(shell find include -name '*.h')

check:
	@printf "$(BLUE)Running$(RESET) cppcheck\n"
	@cppcheck $(SRCS)
	@printf "$(BLUE)Running$(RESET) clang-tidy\n"
	@clang-tidy $(SRCS) -- $(CFLAGS)

run: $(TARGET)
	@printf "$(GREEN)Running$(RESET) $(TARGET)\n"
	@./$(TARGET)

debug:
	@$(MAKE) DEBUG=1

help:
	@echo "Available targets:"
	@echo "  all       - Build the project"
	@echo "  test      - Build and run tests"
	@echo "  coverage  - Run tests with coverage"
	@echo "  clean     - Remove objects"
	@echo "  fclean    - Full clean"
	@echo "  re        - Rebuild"
	@echo "  format    - Run clang-format"
	@echo "  check     - Static analysis"
	@echo "  run       - Run the program"
	@echo "  debug     - Debug build"
	@echo "  help      - Show this help"

.PHONY: all clean fclean re test coverage format check run debug help

