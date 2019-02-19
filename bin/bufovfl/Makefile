bufovfl: bufovfl_w_check bufovfl_ret bufovfl_call 
bufovfl_w_check: bufovfl_w_check.c
	gcc -m64 -o bufovfl_w_check bufovfl_w_check.c -fno-stack-protector
bufovfl_ret: bufovfl_ret.c
	gcc -m64 -o bufovfl_ret bufovfl_ret.c -fno-stack-protector
bufovfl_jmp: bufovfl_jmp.c
	gcc -m64 -o bufovfl_jmp bufovfl_jmp.c -fno-stack-protector   ## so the local buffer won't be reordered
bufovfl_call: bufovfl_call.c
	gcc -m64 -o bufovfl_call bufovfl_call.c -fno-stack-protector ## so the local buffer won't be reordered
clean:
	rm bufovfl_w_check bufovfl_ret bufovfl_jmp bufovfl_call
