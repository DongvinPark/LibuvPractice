#!/bin/bash

set -e  # Exit immediately on error

echo "🔧 Creating build directory..."
mkdir -p build
cd build

echo "⚙️  Running CMake..."
cmake ..

echo "🔨 Building project..."
make

echo "🚀 Running server..."
./LibuvPractice

