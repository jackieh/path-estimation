#!/bin/bash
find src \( -name "*.h" -o -name "*.cc" \) -exec bash -c \
    'python utils/cpplint.py --root=src --filter=-legal/copyright {}' \;
