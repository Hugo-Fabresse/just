#!/bin/bash
# Add GPLv3 license headers to all source files

set -e

HEADER='/*
 * JUST - A lightweight version control system
 * Copyright (C) 2026 Hugo Fabresse
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
'

echo "Adding GPLv3 license headers..."

# Find all .c and .h files
count=0
find src include tests -type f \( -name "*.c" -o -name "*.h" \) 2>/dev/null | while read file; do
    # Skip empty files
    if [ ! -s "$file" ]; then
        continue
    fi

    # Check if header already exists
    if grep -q "GNU General Public License" "$file" 2>/dev/null; then
        echo "   → Skipping $file (already has header)"
    else
        echo "   → Adding header to $file"
        echo "$HEADER" | cat - "$file" > "$file.tmp" && mv "$file.tmp" "$file"
        count=$((count + 1))
    fi
done

echo "License headers added to $count file(s)!"

