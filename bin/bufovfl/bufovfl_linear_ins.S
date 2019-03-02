.globl main
.type main, @function
main:
    push %rbp;
    mov %rsp, %rbp;
    sub $0x50, %rsp;
    lea 0x8(%rsp), %r8; // char buf[0x50] ;

    // read(0, buf, 0x30)
    mov $0x40, %rdx;
	mov %r8, %rsi;
    mov $0x0, %rdi ;
    mov $0x0, %rax;
    syscall ;

    xor %r10, %r10;      // int i = 0;  
linear_cpy_loop:

    movq %r8, %r9 ; 	// char* r9 = buf ;
    add %r10, %r9 ;     // r9 += i ;
    mov (%r9), %bl ;    // char bl = *r9 ;

 	mov %bl, 0x48(%r8, %r10, 1); // buf + 0x48 + i  = bl ;
    add $0x1, %r10;  // i++
	
	cmp  $0x20, %r10;	// i < 20 
	jle linear_cpy_loop ;

	mov $0x60, %rdx;
	mov %r8, %rsi;
	mov $0x1, %rdi;
	mov $0x1, %rax;
	syscall ;	
	mov $0x0, %eax;
    leaveq;
    ret ;

