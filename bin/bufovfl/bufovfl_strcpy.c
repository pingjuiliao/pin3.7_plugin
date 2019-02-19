#include<stdio.h>
#include<string.h>

int
main( int argc, char** argv ) {
    char dst[50];
    char src[100];
    memset(dst, 0, sizeof(dst));
    memset(src, 0, sizeof(src));
    scanf("%s", src);
    strcpy(dst, src);
    
    return 0;
}
