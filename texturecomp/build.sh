#!/bin/bash

clang++ -g3 -static -Wno-writable-strings main.cc -o tcomp -isystem ~/include -lstb
RETURN=$?

exit $RETURN

[[ -z "$RETURN" ]] && tcomp
