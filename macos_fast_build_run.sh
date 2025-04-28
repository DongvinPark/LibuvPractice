#!/bin/bash

set -e  # Exit immediately if any command fails

echo "🔧 Creating build directory..."
mkdir -p build
cd build

echo "⚙️  Running CMake..."
# Prefer Ninja if available, fallback to Make
if command -v ninja &> /dev/null; then
    echo "🛠️  Ninja detected, using Ninja generator"
    cmake -G Ninja ..
else
    echo "🛠️  Ninja not found, using default (Unix Makefiles)"
    cmake ..
fi

echo "🔨 Building project..."
cmake --build .

echo "🚀 Running server..."
./LibuvPractice