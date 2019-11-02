#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int
main(int argc ,char** argv) {
    int r ;
    int digit ; 
    char buf[16] ;

    void* map_area ;
    
    memset(buf, 0, sizeof(buf)) ;
    if ((r = read(0, buf, sizeof(buf))) < 0 ) {
        fprintf(stderr, "read failure\n") ;
        exit(-1) ;
    }
    digit = atoi(buf);
    map_area = mmap((void *)0x666666660000, 0x1000, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,0,0) ;
    // memset(map_area, 0, 0x1000) ;

    *((int *) map_area) = digit ;
    printf("digit == %d\n", digit) ;
    printf("map_area : %p\n", map_area) ;
    printf("*map_area : %d\n", ((int * )map_area)[0] ) ;

}
