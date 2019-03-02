#include<stdio.h>
#include<string.h>
#include<unistd.h>

char buf[128];

void
vuln(void) {
    char dst[32];
    memset(dst, 0, sizeof(dst));
    read(0, buf, sizeof(buf));
    strcpy(dst, buf);
    puts(dst);
    return ;
}

int
main( int argc, char** argv ) {
    memset(buf, 0, sizeof(buf));
    vuln();
    return 0;
}
