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
