#include<stdio.h>
#include<unistd.h>


void helloworld(void);

int
main(int argc, char** argv) {
    void *func_ptr ;
    char buf[50];
    func_ptr = &helloworld;
    read(0, buf, 100);
    ( (void(*)()) func_ptr)();
    return 0;
}

void
helloworld(void) {
    puts("hello world");
    return ;
}
