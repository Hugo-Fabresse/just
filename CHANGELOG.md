# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

- Nothing yet.

---

## [0.1.0] - 2026-01-28

### Added
- Initial project repository and modular C architecture.
- Build system:
  - Makefile with `release` and `debug` build modes.
- Code quality tooling:
  - `clang-format` configuration for consistent code style.
  - `clang-tidy` for static analysis.
  - `cppcheck` integration.
  - `.editorconfig` for cross-editor consistency.
- Git workflow automation via hooks:
  - `pre-commit`: formatting and static analysis checks.
  - `commit-msg`: conventional commit validation.
  - `prepare-commit-msg`: commit message template.
  - `pre-push`: build and test validation.
- Development scripts:
  - `setup-dev.sh`: one-command development environment setup.
  - `install-hooks.sh`: automated Git hooks installation.
  - `add-license-headers.sh`: GPLv3 license header enforcement.
- Comprehensive `.gitignore` tailored for C projects.
- GPLv3 license to guarantee long-term software freedom.
- Community and contribution infrastructure:
  - Issue templates (bug, feature, question, task).
  - Pull request template.
  - Contributing guidelines.
  - Code of conduct.

### Changed
- Nothing.

### Fixed
- Nothing.

---

[unreleased]: https://github.com/Hugo-Fabresse/just/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/Hugo-Fabresse/just/releases/tag/v0.1.0

