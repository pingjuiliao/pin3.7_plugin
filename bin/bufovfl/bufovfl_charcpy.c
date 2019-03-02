#include<stdio.h>
#include<string.h>
#include<unistd.h>
#define BUFSIZE 1024
void vuln(char* src);
int i ;
int
main( int argc, char** argv ) {
    char src_buf[BUFSIZE];
    memset(src_buf, 0, sizeof(src_buf));
    read(0, src_buf,  sizeof(src_buf) - 1);
    vuln(src_buf);
    return 0;
}

void
vuln(char* src) {
    
    char dst[32];
    for ( i = 0 ; i < BUFSIZE ; ++i ) {
        dst[i] = src[i] ;
    }
    printf("The string is %s\n", dst);
    return ;
}
