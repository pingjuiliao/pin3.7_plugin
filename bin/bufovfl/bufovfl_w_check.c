#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

int
main(int argc , char** argv) {
    int i ;
    char buf[10];
    memset(buf, 0, sizeof(buf));
    read(0, buf, 4);
    i = atoi(buf);
    if ( i == 20 ) {
        read(0, buf, 50);
    } else {
        puts("no vuln");
    }
    return 0;
}  
