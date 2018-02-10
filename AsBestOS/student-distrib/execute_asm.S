.data 
	VIRT_ADDR = 0x08048000
	INT_FLAG_BIT = 0x200
	STACK_PTR = 0x083FFFFC

# global variables

.globl execute_wrapper, halty_boi


execute_wrapper:
	#movl 8(%ebp), %ebx
	movw $0x2B, %ax # setting DS

	pushl $0x2B # Stack segment
	movl $STACK_PTR, %eax
	pushl %eax # New  ESP

	pushfl # EFLAGS
	popl %ecx
	orl $INT_FLAG_BIT, %ecx
	pushl %ecx
	
	pushl $0x23 # Code Segment
	# movl 8(%ebp), %ebx
	pushl %ebx # argument passed in(program instruction entry point)

	iret

halty_boi:
	leave
	ret
