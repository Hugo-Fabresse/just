# just

`just` is a version control system written in C.

This project is currently in **early development** and serves primarily as a foundation for architecture, tooling, and workflow discipline. At this stage, the program only prints a placeholder message (`Hello, world!`) and does **not** implement any version control features yet.

---

## Status

- **Version**: 0.1.0
- **Maturity**: Experimental / non-functional
- **Audience**: Developers interested in low-level tooling, C architecture, and version control internals

---

## Goals

The long-term objective of `just` is to explore and implement a minimal, explicit, and well-structured version control system with a strong focus on:

- Clear internal architecture
- Strict tooling and code quality
- Deterministic behavior
- Low-level control (C, no hidden abstractions)

At this stage, the focus is on **infrastructure first**, features later.

---

## Getting Started

### Prerequisites

#### Required
- **clang** (or gcc) - C compiler
- **make** - Build system

#### Recommended for development
- **clang-format** - Code formatting
- **clang-tidy** - Static analysis
- **cppcheck** - Additional static checks

#### Installation

**On Ubuntu/Debian:**
```sh
sudo apt update
sudo apt install -y clang make clang-format clang-tidy cppcheck
```

**On macOS (with Homebrew):**
```sh
brew install llvm make
brew install clang-format cppcheck
```

**On Arch Linux:**
```sh
sudo pacman -S --needed clang clang-format clang-tidy cppcheck make
```

---

### Quick Setup

The fastest way to set up your development environment:
```sh
# Clone the repository
git clone https://github.com/Hugo-Fabresse/just.git
cd just

# Run automated setup (installs hooks, builds project, checks dependencies)
./scripts/setup-dev.sh
```

This will:
- ✅ Check all prerequisites
- ✅ Install git hooks
- ✅ Build the project
- ✅ Verify everything works

---

## Building

### Available Build Targets
```sh
make              # Build release version (optimized)
make all          # Same as 'make'
make debug        # Build with debug symbols and no optimization
make clean        # Remove object files
make fclean       # Remove all build artifacts
make re           # Clean rebuild (fclean + all)
make run          # Build and run the program
make help         # Show all available targets
```

### Build Modes

#### Release Build (default)
```sh
make
# Compiler flags: -O2 -DNDEBUG
# Output: bin/just
```

#### Debug Build
```sh
make debug
# Compiler flags: -g -O0 -DDEBUG
# Output: bin/just (with debug symbols)
```

The debug build includes:
- `-g` - Debug symbols for GDB
- `-O0` - No optimization (easier to debug)
- `-DDEBUG` - Enables debug-only code blocks

### Running
```sh
# Run the program
./bin/just

# Or build and run in one command
make run
```

**Current output:** `Hello, world!` (placeholder)

---

## Code Quality & Tooling

The project enforces **strict development standards** through automated tooling.

### Static Analysis & Formatting
```sh
make format       # Format all code with clang-format
make check        # Run static analysis (cppcheck + clang-tidy)
```

Configuration files:
- [`.clang-format`](.clang-format) - Code formatting rules (LLVM style, 4-space indent)
- [`.clang-tidy`](.clang-tidy) - Linting and static analysis rules
- [`.cppcheck.cfg`](.cppcheck.cfg) - Cppcheck configuration
- [`.editorconfig`](.editorconfig) - Editor-agnostic settings

### Git Hooks

Git hooks automatically enforce code quality **before commits and pushes**.

#### Install hooks:
```sh
./scripts/install-hooks.sh
```

#### Hook Details:

| Hook | When | What it does |
|------|------|-------------|
| **pre-commit** | Before each commit | • Ensures newline at EOF<br>• Runs `clang-format`<br>• Runs `cppcheck`<br>• Runs `clang-tidy` |
| **commit-msg** | After commit message | • Validates [Conventional Commits](https://www.conventionalcommits.org/) format<br>• Enforces lowercase, 72 char limit |
| **prepare-commit-msg** | Before editor opens | • Provides commit message template |
| **pre-push** | Before pushing | • Checks working directory is clean<br>• Builds project<br>• Verifies code formatting<br>• Runs static analysis<br>• Checks for TODOs/FIXMEs |

See [`.githooks/`](.githooks/) directory for implementation details.

#### Commit Message Format

We follow [Conventional Commits](https://www.conventionalcommits.org/):
```
<type>: <description>

Types: feat, fix, refactor, style, test, docs, build, ci, chore
```

**Examples:**
```
feat: add basic repository initialization
fix: prevent segfault in object parser
docs: update installation instructions
```

---

## Development Workflow

### 1. Setup Environment
```sh
./scripts/setup-dev.sh
```

### 2. Create a Branch
```sh
git checkout -b feature/your-feature
```

### 3. Make Changes
```sh
# Edit code
vim src/commands/init/init.c

# Format before committing
make format

# Check for issues
make check
```

### 4. Commit
```sh
git add .
git commit
# Hooks will automatically validate your code and commit message
```

### 5. Push
```sh
git push origin feature/your-feature
# Pre-push hook will build and validate everything
```

---

## Documentation

- **[CHANGELOG.md](CHANGELOG.md)** - Version history and release notes
- **[CONTRIBUTING.md](.github/CONTRIBUTING.md)** - Contribution guidelines
- **[CODE_OF_CONDUCT.md](.github/CODE_OF_CONDUCT.md)** - Community standards
- **Man pages** - `docs/man/` (coming soon)

---

## License

This project is licensed under the **GNU General Public License v3.0**.

See [LICENSE](LICENSE) for the full text.

**In short:**
- ✅ You can use, modify, and distribute this software
- ✅ Modifications must also be open source (copyleft)
- ✅ No warranty provided

---

## Support & Community

- **Issues**: [GitHub Issues](https://github.com/Hugo-Fabresse/just/issues)
- **Discussions**: [GitHub Discussions](https://github.com/Hugo-Fabresse/just/discussions)

