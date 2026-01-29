#!/bin/bash
# scripts/release.sh - Automate the release process for JUST

set -e

BLUE="\033[0;34m"
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
RESET="\033[0m"

# Usage
if [ $# -ne 1 ]; then
    echo "Usage: $0 <version>"
    echo ""
    echo "Example: $0 0.2.0"
    echo ""
    echo "This will:"
    echo "  1. Verify clean working directory"
    echo "  2. Run all checks (build, format, analysis)"
    echo "  3. Prompt to update CHANGELOG.md"
    echo "  4. Create git tag v<version>"
    echo "  5. Push tag to origin"
    echo "  6. Provide instructions for GitHub release"
    exit 1
fi

VERSION=$1
TAG="v$VERSION"

printf "${BLUE}╔════════════════════════════════════════╗${RESET}\n"
printf "${BLUE}║${RESET}           Preparing Release ${TAG}           ${BLUE}║${RESET}\n"
printf "${BLUE}╚════════════════════════════════════════╝${RESET}\n"
printf "\n"

# Step 1: Check git status
printf "${BLUE}→${RESET} Checking git status...\n"
if ! git diff-index --quiet HEAD --; then
    printf "${RED}✗ Error: Working directory is not clean${RESET}\n"
    printf "  Commit or stash your changes first.\n"
    exit 1
fi
printf "${GREEN}✓${RESET} Working directory is clean\n"

# Step 2: Ensure we're on main
printf "${BLUE}→${RESET} Checking branch...\n"
CURRENT_BRANCH=$(git symbolic-ref --short HEAD)
if [ "$CURRENT_BRANCH" != "main" ]; then
    printf "${YELLOW}⚠ Warning: Not on main branch (currently on $CURRENT_BRANCH)${RESET}\n"
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi
printf "${GREEN}✓${RESET} On branch $CURRENT_BRANCH\n"

# Step 3: Pull latest changes
printf "${BLUE}→${RESET} Pulling latest changes...\n"
git pull origin "$CURRENT_BRANCH"
printf "${GREEN}✓${RESET} Up to date with origin\n"

# Step 4: Check if tag already exists
printf "${BLUE}→${RESET} Checking if tag already exists...\n"
if git rev-parse "$TAG" >/dev/null 2>&1; then
    printf "${RED}✗ Error: Tag $TAG already exists${RESET}\n"
    exit 1
fi
printf "${GREEN}✓${RESET} Tag $TAG is available\n"

# Step 5: Run checks
printf "\n${BLUE}→${RESET} Running code quality checks...\n"

# Clean build
printf "  ${BLUE}→${RESET} Building release version...\n"
if ! make fclean > /dev/null 2>&1; then
    printf "${YELLOW}⚠ Warning: Clean failed${RESET}\n"
fi

if ! make > /dev/null 2>&1; then
    printf "${RED}✗ Build failed${RESET}\n"
    exit 1
fi
printf "${GREEN}✓${RESET} Build successful\n"

# Static analysis
printf "  ${BLUE}→${RESET} Running static analysis...\n"
if ! make check > /dev/null 2>&1; then
    printf "${RED}✗ Static analysis failed${RESET}\n"
    exit 1
fi
printf "${GREEN}✓${RESET} Static analysis passed\n"

# Tests (if available)
if make -n test > /dev/null 2>&1; then
    printf "  ${BLUE}→${RESET} Running tests...\n"
    if ! make test > /dev/null 2>&1; then
        printf "${RED}✗ Tests failed${RESET}\n"
        exit 1
    fi
    printf "${GREEN}✓${RESET} Tests passed\n"
fi

# Step 6: Prompt to update CHANGELOG
printf "\n${YELLOW}⚠ IMPORTANT: Update CHANGELOG.md${RESET}\n"
printf "  1. Move [Unreleased] items to [${VERSION}] - $(date +%Y-%m-%d)\n"
printf "  2. Add empty [Unreleased] section\n"
printf "  3. Update comparison links at the bottom\n"
printf "\n"
printf "Example:\n"
printf "${BLUE}## [Unreleased]\n"
printf "\n"
printf "### Added\n"
printf "- Nothing yet\n"
printf "\n"
printf "## [${VERSION}] - $(date +%Y-%m-%d)\n"
printf "[Your changes here]${RESET}\n"
printf "\n"

read -p "Press Enter when CHANGELOG.md is updated..." -r

# Verify CHANGELOG was updated
if ! grep -q "\[${VERSION}\]" CHANGELOG.md; then
    printf "${RED}✗ Error: CHANGELOG.md doesn't contain [${VERSION}]${RESET}\n"
    exit 1
fi
printf "${GREEN}✓${RESET} CHANGELOG.md updated\n"

# Step 7: Commit CHANGELOG changes
printf "${BLUE}→${RESET} Committing CHANGELOG.md...\n"
git add CHANGELOG.md
if ! git diff --cached --quiet; then
    git commit -m "chore: prepare release ${TAG}"
    printf "${GREEN}✓${RESET} CHANGELOG committed\n"
else
    printf "${YELLOW}⚠${RESET} No changes to commit\n"
fi

# Step 8: Create annotated tag
printf "${BLUE}→${RESET} Creating git tag ${TAG}...\n"

# Extract release notes from CHANGELOG
RELEASE_NOTES=$(sed -n "/## \[${VERSION}\]/,/## \[/p" CHANGELOG.md | sed '$d' | tail -n +2)

git tag -a "$TAG" -m "Release ${TAG}

${RELEASE_NOTES}"

printf "${GREEN}✓${RESET} Tag ${TAG} created\n"

# Step 9: Push changes and tag
printf "${BLUE}→${RESET} Pushing to origin...\n"
git push origin "$CURRENT_BRANCH"
git push origin "$TAG"
printf "${GREEN}✓${RESET} Pushed to origin\n"

# Step 10: Instructions for GitHub Release
printf "\n${GREEN}╔════════════════════════════════════════╗${RESET}\n"
printf "${GREEN}║${RESET}     Release ${TAG} prepared! ✓         ${GREEN}║${RESET}\n"
printf "${GREEN}╚════════════════════════════════════════╝${RESET}\n"
printf "\n"
printf "${YELLOW}Next steps:${RESET}\n"
printf "  1. Go to: ${BLUE}https://github.com/Hugo-Fabresse/just/releases/new${RESET}\n"
printf "  2. Choose tag: ${BLUE}${TAG}${RESET}\n"
printf "  3. Title: ${BLUE}${TAG}${RESET}\n"
printf "  4. Copy release notes from CHANGELOG.md\n"
printf "  5. Check 'Set as latest release'\n"
printf "  6. Click 'Publish release'\n"
printf "\n"
printf "Or use GitHub CLI:\n"
printf "  ${BLUE}gh release create ${TAG} --title '${TAG}' --notes-file <(sed -n '/## \[${VERSION}\]/,/## \[/p' CHANGELOG.md | sed '\$d' | tail -n +2)${RESET}\n"
printf "\n"

# Cleanup
make clean > /dev/null 2>&1

