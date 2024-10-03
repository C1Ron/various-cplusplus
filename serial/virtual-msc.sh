#!/bin/bash

## Handling multiple instances and cleanup ==========================================
LOCKFILE="/tmp/virtual-msc.lock"

# Function to handle cleanup on exit
cleanup() {
    echo "Cleaning up..."
    # Remove the lock file
    rm -f "$LOCKFILE"
    echo "Lock file removed."
    exit 0
}

# Trap the exit signal and remove the lock file
trap cleanup EXIT

# Check if the lock file exists and if the process is still running
if [ -f "$LOCKFILE" ]; then
    LOCKPID=$(cat "$LOCKFILE")
    
    # Check if the process with the stored PID is still running
    if ps -p "$LOCKPID" > /dev/null 2>&1; then
        echo "Another instance of the script is already running (PID $LOCKPID). Exiting..."
        exit 1
    else
        echo "Lock file exists, but the process is not running. Cleaning up stale lock file."
        rm -f "$LOCKFILE"
    fi
fi

# Create the lock file and store the current script's PID in it
echo $$ > "$LOCKFILE"

# Your script's main logic goes here
echo "Script is running with PID $$..."

## Main script logic =================================================================

# Ensure the device is passed as an argument
if [ -z "$1" ]; then
    echo "Usage: $0 <device>"
    exit 1
fi

# Device (e.g., /dev/pts/9) for reading and writing
device=$1
nBytes=48

# Calculate CRC
calculate_crc() 
{
    local response="$1"
    local total=0

    for byte in $(echo "$response" | sed 's/0x/\n0x/g' | grep '0x'); do
        total=$(( (total + byte) % 65536 ))  # uint_16 sum
    done

    local lowByte=$(( total & 0x00FF ))
    local highByte=$(( (total & 0xFF00) >> 8 ))

    local crc=$(( (highByte + lowByte) % 256 ))

    printf "0x%02x" "$crc"
}

while true; do
    # Read from device
    output=$(dd if="$device" bs=$nBytes count=1 2>/dev/null | xxd -p)

    # Check if data was received
    if [ -z "$output" ]; then
        echo "No data received. Still waiting..."
        continue  # Go back to waiting for the next command
    fi

    # Display the received command in hex format (space-delimited)
    echo "Received data from $device:"
    echo "$output" | awk '{
        for (i=1; i<=length($0); i+=2) {
            byte = substr($0, i, 2)
            printf "0x%s", byte
            if (i+2 <= length($0)) {
                printf " "
            }
        }
        printf "\n"
    }'

    # Prompt the user for the response 
    echo "Respond space-delimited hex bytes or 'quit' to exit. "
    echo "    ackByte payloadLength payload"
    read -r userResponse

    # Check if the user wants to quit
    if [[ "$userResponse" == "quit" || "$userResponse" == "exit" ]]; then
        echo "Exiting the script."
        break
    fi

    # Split user response into components
    # Extract the payload length (second byte in the user response)
    payloadLengthHex=$(echo "$userResponse" | awk '{print $2}' | sed 's/0x//')
    payloadLength=$((16#$payloadLengthHex))

    # Validate the payload length
    numPayloadBytes=$(echo "$userResponse" | awk '{print NF-2}')
    if [[ "$numPayloadBytes" -ne "$payloadLength" ]]; then
        echo "Error: The number of payload bytes ($numPayloadBytes) does not match the specified payload length ($payloadLength)."
        continue
    fi

    crc=$(calculate_crc "$userResponse")
    fullResponse="$userResponse $crc"

    echo "Full response: $fullResponse"

    cleanedResponse=$(echo "$fullResponse" | sed 's/0x//g; s/ //g')

    echo "$cleanedResponse" | xxd -r -p > /tmp/response.bin

    dd if=/tmp/response.bin of="$device" bs=1 count=$(wc -c < /tmp/response.bin) 2>/dev/null

    echo "Response sent to $device"
    echo "-----------------------------"
done
