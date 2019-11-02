#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <unistd.h>

#define ARR_SIZE 5
void
error(char* str, ...) {
    va_list args;
    va_start(args, str);

    vprintf(str, args);

    va_end(args);
    exit(-1) ;
}

int
main(int argc, char** argv) {
    int r ;
    int32_t arr[ARR_SIZE] ;
    void *map_area ;
    unsigned int seed ;
    if ( (r = read(0, &seed, sizeof(seed))) < 0 ) {
        error("read failure\n") ; 
    }
    srand(seed) ;
    map_area = mmap( (void *)0x666666660000, 0x1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0) ;
    printf("map_area: address == %p\n", map_area) ;
    memset(map_area, 0, 0x1000) ;
    *((int *) map_area) = rand() ;
    printf("random number %d\n", *((int *) map_area) ) ;
    return 0;
}
