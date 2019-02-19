#include<stdio.h>
#include<string.h>
#include<unistd.h>

int
main(int argc, char** argv) {
    char buf[50];
    memset(buf, 0, sizeof(buf));
    read(0, buf, 100);
    return 0;
}
