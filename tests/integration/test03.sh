#!/bin/bash

set -e

TRIE=$(mktemp)
COMPILE_INPUT=$(mktemp)
COMPILE_OUTPUT=$(mktemp)
QUERY_INPUT=$(mktemp)
QUERY_OUTPUT=$(mktemp)

cat >"$COMPILE_INPUT" <<EOF
foo
baz
ahoj
EOF

cat >"$COMPILE_OUTPUT" <<EOF
Inserted 3 items
EOF

cat >"$QUERY_INPUT" << EOF
foo
baz
non
EOF

cat >"$QUERY_OUTPUT" <<EOF
Found
Found
Not found
EOF

make

valgrind -q ./list-compile -e "$COMPILE_INPUT" "$TRIE" \
    | diff -q - "$COMPILE_OUTPUT"

valgrind -q ./list-query $TRIE <"$QUERY_INPUT" \
    | diff -q - "$QUERY_OUTPUT"

rm -f $TRIE $COMPILE_INPUT $COMPILE_OUTPUT $QUERY_INPUT $QUERY_OUTPUT