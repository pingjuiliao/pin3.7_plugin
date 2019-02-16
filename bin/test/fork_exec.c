#include<stdio.h>
#include<unistd.h>

int
main(int argc, char** argv ) {
    pid_t pid ;

    while (1) {
        pid = fork();
        if ( pid == 0 ) {
            puts("i am the child");
        } 
            puts("I am the parent");
        }
    }
}
