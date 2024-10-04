#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[])
{
    char* cmd = argv[1];
    if (strcmp(cmd, "read") == 0) {
        printf("Read command received\n");
    } else if (strcmp(cmd, "write") == 0) {
        printf("Write command received, with argument '%s'\n", argv[2]);
    } else {
        printf("Neither read nor write was found. \n\t cmd = %s\n", cmd);
    }
    return 0;
}