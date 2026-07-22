# File Integrity Monitor

Command-line tool written in C++ to monitor file integrity using SHA-256 hashes. It detects unauthorized modifications, new files, and deletions by comparing the current state against a baseline.

## Features
- Generate a baseline of SHA-256 hashes for all files in a directory.
- Verify the current state of files against the baseline.
- Detect modified, new, and missing files.

## Build
cmake -B build && cmake --build build

## Usage
# Generate a baseline
./build/file-integrity-monitor generate /path/to/directory

# Verify integrity
./build/file-integrity-monitor verify /path/to/directory

This project is for educational purposes and auditing on systems you own or have explicit permission to monitor.
