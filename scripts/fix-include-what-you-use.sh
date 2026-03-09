#!/usr/bin/env bash

PROJ_DIR="$( dirname -- "${BASH_SOURCE[0]}" )/.."

cmake --build "$PROJ_DIR/build" --clean-first | iwyu-fix-includes
