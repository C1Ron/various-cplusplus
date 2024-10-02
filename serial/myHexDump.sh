#!/bin/bash

# Check if nBytes is passed as an argument
if [ -z "$1" ]; then
    echo "Usage: $0 nBytes"
    exit 1
fi

# Number of bytes to read
nBytes=$1

# Device (e.g., /dev/pts/3) - change this if needed
device=$2

# Use 'timeout' to enforce a 5-second limit
output=$(timeout 5 dd if="$device" bs="$nBytes" count=1 2>/dev/null | xxd -p)

# Check if the timeout expired or no data was received
if [ -z "$output" ]; then
    echo "No data received within 5 seconds."
    exit 1
fi

# Use awk to format the hex string output into [0xab, 0xcd, ...]
echo "$output" | awk '{
    # Split the hex string into pairs of characters
    for (i=1; i<=length($0); i+=2) {
        byte = substr($0, i, 2)
        # Print each byte in the form 0xab
        printf "0x%s", byte
        if (i+2 <= length($0)) {
            printf ", "
        }
    }
    printf "\n"
}'
