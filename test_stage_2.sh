#!/bin/bash

# Delete test_dir if it exists
rm -rf test_dir

# Create test_dir and store full path
TEST_DIR=$(pwd)/test_dir
mkdir $TEST_DIR

# Store path to your_program.sh
PROGRAM_PATH=$(pwd)/your_program.sh

# Navigate to test_dir
cd $TEST_DIR

# Run initialization
$PROGRAM_PATH init

echo "============================"
# Create test file
echo "hello world" > test.txt

# Pipe hash-object output to cat_file
HASH=$(git hash-object -w test.txt)
$PROGRAM_PATH cat-file -p $HASH
