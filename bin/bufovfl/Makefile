bufovfl: bufovfl_w_check bufovfl_ret bufovfl_call bufovfl_strcpy bufovfl_scanf bufovfl_charcpy bufovfl_linear_ins bufovfl_memcpy non_bufovfl_able
bufovfl_w_check: bufovfl_w_check.c
	gcc -m64 -o bufovfl_w_check bufovfl_w_check.c -fno-stack-protector
bufovfl_ret: bufovfl_ret.c
	gcc -m64 -o bufovfl_ret bufovfl_ret.c -fno-stack-protector
bufovfl_jmp: bufovfl_jmp.c
	gcc -m64 -o bufovfl_jmp bufovfl_jmp.c -fno-stack-protector   ## so the local buffer won't be reordered
bufovfl_call: bufovfl_call.c
	gcc -m64 -o bufovfl_call bufovfl_call.c -fno-stack-protector ## so the local buffer won't be reordered
bufovfl_strcpy: bufovfl_strcpy.c
	gcc -m64 -o bufovfl_strcpy bufovfl_strcpy.c -fno-stack-protector 
bufovfl_scanf: bufovfl_scanf.c
	gcc -m64 -o bufovfl_scanf bufovfl_scanf.c -fno-stack-protector
bufovfl_charcpy: bufovfl_charcpy.c
	gcc -m64 -o bufovfl_charcpy bufovfl_charcpy.c -fno-stack-protector
bufovfl_linear_ins: bufovfl_linear_ins.S
	gcc -m64 -c bufovfl_linear_ins.S -o bufovfl_linear_ins.o
	gcc -m64 -o bufovfl_linear_ins bufovfl_linear_ins.o
bufovfl_memcpy: bufovfl_memcpy.c
	gcc -m64 -o bufovfl_memcpy bufovfl_memcpy.c -fno-stack-protector
non_bufovfl_able: non_bufovfl_able.c
	gcc -m64 -o non_bufovfl_able non_bufovfl_able.c
clean:
	rm bufovfl_w_check bufovfl_ret bufovfl_jmp bufovfl_call
