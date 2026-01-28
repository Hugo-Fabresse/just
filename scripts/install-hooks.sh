#!/bin/sh
# Install git hooks for JUST

set -e

HOOKS_DIR=".githooks"
GIT_HOOKS_DIR=".git/hooks"

echo "Installing git hooks..."

if [ ! -d "$HOOKS_DIR" ]; then
    echo " Error: $HOOKS_DIR directory not found"
    exit 1
fi

# Copy hooks
for hook in "$HOOKS_DIR"/*; do
    hook_name=$(basename "$hook")
    echo "   → Installing $hook_name"
    cp "$hook" "$GIT_HOOKS_DIR/$hook_name"
    chmod +x "$GIT_HOOKS_DIR/$hook_name"
done

