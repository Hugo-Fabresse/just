#!/bin/sh
# Setup development environment for JUST

set -e

BLUE="\033[0;34m"
GREEN="\033[0;32m"
RED="\033[0;31m"
RESET="\033[0m"

printf "${BLUE}╔════════════════════════════════════════╗${RESET}\n"
printf "${BLUE}║${RESET}         JUST Development Setup         ${BLUE}║${RESET}\n"
printf "${BLUE}╚════════════════════════════════════════╝${RESET}\n"
printf "\n"

# Check prerequisites
printf "${BLUE}→${RESET} Checking prerequisites...\n"

command -v clang >/dev/null 2>&1 || {
    printf "${RED}✗ Error: clang not found${RESET}\n"
    exit 1
}
printf "${GREEN}✓${RESET} clang found\n"

command -v make >/dev/null 2>&1 || {
    printf "${RED}✗ Error: make not found${RESET}\n"
    exit 1
}
printf "${GREEN}✓${RESET} make found\n"

command -v clang-format >/dev/null 2>&1 || {
    printf "${RED}⚠ Warning: clang-format not found (optional)${RESET}\n"
}

command -v clang-tidy >/dev/null 2>&1 || {
    printf "${RED}⚠ Warning: clang-tidy not found (optional)${RESET}\n"
}

command -v cppcheck >/dev/null 2>&1 || {
    printf "${RED}⚠ Warning: cppcheck not found (optional)${RESET}\n"
}

# Install git hooks
printf "\n"
printf "${BLUE}→${RESET} Installing git hooks...\n"
./scripts/install-hooks.sh

# Build the project
printf "\n"
printf "${BLUE}→${RESET} Building JUST...\n"
make fclean > /dev/null 2>&1 || true
if make; then
    printf "${GREEN}✓${RESET} Build successful\n"
else
    printf "${RED}✗${RESET} Build failed\n"
    exit 1
fi
make fclean > /dev/null 2>&1

printf "\n"
printf "${GREEN}╔════════════════════════════════════════╗${RESET}\n"
printf "${GREEN}║${RESET}    Development environment ready! ✓    ${GREEN}║${RESET}\n"
printf "${GREEN}╚════════════════════════════════════════╝${RESET}\n"
printf "\n"
printf "Next steps:\n"
printf "  • make          - Build the project\n"
printf "  • make debug    - Build with debug symbols\n"
printf "  • make format   - Format code\n"
printf "  • make check    - Run static analysis\n"
printf "  • make test     - Run tests\n"
printf "  • make help     - Show all available targets\n"

