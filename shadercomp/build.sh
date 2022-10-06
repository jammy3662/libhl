#!/bin/bash

clang++ main.cc -o scomp -isystem ~/include
RETURN=$?

[[ -z "$RETURN" ]] && scomp
