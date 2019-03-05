#include<stdio.h>
#include<string.h>
#include<unistd.h>

int
main(int argc, char** argv) {
    char buf[20];
    memset(buf, 0, sizeof(buf));
    puts("Please type your name: ");
    read(0, buf, 19);
    puts(buf);
    return 0 ;
}
