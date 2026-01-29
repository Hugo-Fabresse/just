#!/bin/bash
# scripts/release.sh - Automate the release process for JUST

set -euo pipefail

BLUE="\033[0;34m"
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
RESET="\033[0m"

die() {
    printf "%b✗ %s%b\n" "$RED" "$1" "$RESET" >&2
    exit 1
}

info() {
    printf "%b→ %s%b\n" "$BLUE" "$1" "$RESET"
}

ok() {
    printf "%b✓ %s%b\n" "$GREEN" "$1" "$RESET"
}

warn() {
    printf "%b⚠ %s%b\n" "$YELLOW" "$1" "$RESET"
}

# Usage
if [ $# -ne 1 ]; then
    printf "Usage: %s <version>\n" "$0"
    printf "Example: %s 0.1.0\n" "$0"
    exit 1
fi

VERSION="$1"
TAG="v$VERSION"

printf "%b╔════════════════════════════════════════╗%b\n" "$BLUE" "$RESET"
printf "%b║%b         Preparing Release %-10s   %b║%b\n" "$BLUE" "$RESET" "$TAG" "$BLUE" "$RESET"
printf "%b╚════════════════════════════════════════╝%b\n\n" "$BLUE" "$RESET"

# 1. Git state checks
info "Checking git status..."
git diff-index --quiet HEAD -- || die "Working directory is not clean"
ok "Working directory is clean"

info "Checking branch..."
CURRENT_BRANCH="$(git symbolic-ref --short HEAD)"
[ "$CURRENT_BRANCH" = "main" ] || {
    warn "Not on main branch (currently on $CURRENT_BRANCH)"
    read -r -p "Continue anyway? (y/N) " reply
    [[ "$reply" =~ ^[Yy]$ ]] || exit 1
}
ok "On branch $CURRENT_BRANCH"

info "Pulling latest changes..."
git pull origin "$CURRENT_BRANCH"
ok "Up to date with origin"

info "Checking tag availability..."
git rev-parse "$TAG" >/dev/null 2>&1 && die "Tag $TAG already exists"
ok "Tag $TAG is available"

# 2. Build & checks
info "Running build..."
make fclean >/dev/null 2>&1 || warn "Clean failed"
make >/dev/null || die "Build failed"
ok "Build successful"

info "Running static analysis..."
make check >/dev/null || die "Static analysis failed"
ok "Static analysis passed"

# Optional tests
if make -n test >/dev/null 2>&1; then
    info "Running tests..."
    make test >/dev/null || die "Tests failed"
    ok "Tests passed"
fi

# 3. CHANGELOG validation
info "Validating CHANGELOG.md..."

[ -f CHANGELOG.md ] || die "CHANGELOG.md not found"

grep -q "## \\[$VERSION\\]" CHANGELOG.md \
    || die "CHANGELOG.md does not contain section [$VERSION]"

ok "CHANGELOG.md contains [$VERSION]"

# 4. Commit CHANGELOG if needed
git add CHANGELOG.md
if ! git diff --cached --quiet; then
    git commit -m "chore: prepare release $TAG"
    ok "CHANGELOG committed"
else
    warn "No CHANGELOG changes to commit"
fi

# 5. Create annotated tag
info "Creating annotated tag $TAG..."

RELEASE_NOTES="$(
    sed -n "/## \\[$VERSION\\]/,/^## \\[/p" CHANGELOG.md \
    | sed '$d' \
    | tail -n +2
)"

git tag -a "$TAG" -m "Release $TAG

$RELEASE_NOTES"

ok "Tag $TAG created"

# 6. Push
info "Pushing to origin..."
git push origin "$CURRENT_BRANCH"
git push origin "$TAG"
ok "Pushed branch and tag"

# 7. Final instructions
printf "\n%b╔════════════════════════════════════════╗%b\n" "$GREEN" "$RESET"
printf "%b║%b     Release %-10s ready ✓         %b║%b\n" "$GREEN" "$RESET" "$TAG" "$GREEN" "$RESET"
printf "%b╚════════════════════════════════════════╝%b\n\n" "$GREEN" "$RESET"

printf "%bNext steps:%b\n" "$YELLOW" "$RESET"
printf "  1. Go to: %bhttps://github.com/Hugo-Fabresse/just/releases/new%b\n" "$BLUE" "$RESET"
printf "  2. Choose tag: %b%s%b\n" "$BLUE" "$TAG" "$RESET"
printf "  3. Title: %b%s%b\n" "$BLUE" "$TAG" "$RESET"
printf "  4. Paste release notes from CHANGELOG.md\n"
printf "  5. Publish release\n\n"

printf "GitHub CLI alternative:\n"
printf "  %bgh release create %s --title '%s' --notes \"%s\"%b\n" \
    "$BLUE" "$TAG" "$TAG" "$RELEASE_NOTES" "$RESET"

make clean >/dev/null 2>&1 || true

