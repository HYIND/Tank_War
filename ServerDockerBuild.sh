#!/bin/bash
set -e

echo "=== Cloning Libs ==="
rm -rf Libs
git clone --depth 1 https://github.com/HYIND/Libs.git Libs

echo "=== Building Docker image ==="
sudo docker build -t tank-server:latest -f ./Dockerfile . --progress=plain --no-cache

echo "=== Cleaning up ==="
rm -rf Libs

echo "=== Done ==="