## Compile with the `static` keyword to avoid the `GLIBC_2.34 not found` error:
```
arm-linux-gnueabihf-gcc test.c -o test -static
```
or
```
arm-linux-gnueabihf-g++ test.cpp -o test -static
```

## Transfer file to `/tmp` on FEC by 
```
scp file root@192.168.0.103:/tmp
```

## Login as root: 
```
ssh root@192.168.0.103
```

