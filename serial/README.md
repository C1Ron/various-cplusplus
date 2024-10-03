## Test using `socat`:
```
$ socat -d -d pty,raw,echo=0 pty,raw,echo=0
socat[16875] N PTY is /dev/pts/2
socat[16875] N PTY is /dev/pts/3
```
## Connect the main application to `/dev/pts/2`
```
$ ./main /dev/pts/2
set-register SpeedKp 32
get-register TorqueRef
execute StartMotor
execute-ramp final-speed duration
```
## Read `/dev/pts/3` using `dd` and `xxd`, formatted and properly handled in `myHexDump.sh`
```
$ ./myHexDump.sh 10
```
## Respond to the main application using `echo`
```
$ echo -ne "\xF0\x00\x12" > /dev/pts/3
```

## Using `virtual-msc.sh` 
Assume `socat -d -d pty,raw,echo=0 pty,raw,echo=0` opens two virtual ports: `/dev/pts/8` and `/dev/pts/9`.
Then we can do
```shell
# Shell 1
$ ./main /dev/pts/8
Enter command:

# Shell 2
$ ./virtual-msc.sh /dev/pts/9

# Shell 1
Enter command: get-register SpeedRef # input by user

# Shell 2
Received data from /dev/pts/9:
0x22 0x01 0x04 0x27
Respond space-delimited hex bytes or 'quit' to exit.
    ackByte payloadLength payload

# Shell 1

# Shell 2
0xF0 0x02 0x00 0xFA # Input by user
Full response: 0xF0 0x02 0x00 0xFA 0xed
Response sent to /dev/pts/9
-----------------------------

# Shell 1
0xf0 0x02 0x00 0xfa 0xed
Success. Payload: 64000
```