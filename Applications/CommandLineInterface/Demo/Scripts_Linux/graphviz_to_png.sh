#!/bin/bash
#Helper script. Not intended to run directly.
#Please install graphviz

dot $1 -Tpng -o $1.png
