.globl KB_WRAPPER, RTC_WRAPPER, INTEL_WRAPPER, WRAPPER_0, WRAPPER_1, WRAPPER_2, WRAPPER_3, WRAPPER_4, WRAPPER_5, WRAPPER_6, WRAPPER_7, WRAPPER_8, WRAPPER_9, WRAPPER_10, WRAPPER_11, WRAPPER_12, WRAPPER_13, WRAPPER_14, WRAPPER_15, WRAPPER_16, WRAPPER_17, WRAPPER_18, WRAPPER_19, SYS_CALL_WRAPPER, PIT_WRAPPER

#
# KB_WRAPPER
# Description:  provides the assembly linkage to safely
# 			    call and return from the keyboard
#			  	inturrupt handler
# No inputs and outputs
#
KB_WRAPPER:
	cli
	pushal
	pushfl
	call keyboard_handler
	popfl
	popal
	sti
	iret


#
# RTC_WRAPPER
# Description:  provides the assembly linkage to safely
# 			    call and return from the realtime clock
#			  	inturrupt handler
# No inputs and outputs
#
RTC_WRAPPER:
#	cli
	pushal
#	pushfl
	call rtc_handler
#	popfl
	popal
#	sti
	iret

#
# PIT_WRAPPER
# Description:  provides the assembly linkage to safely
# 			    call and return from the pit
#			  	inturrupt handler
# No inputs and outputs
#
PIT_WRAPPER:
	cli
	pushal
	pushfl
	call pit_handler
	popfl
	popal
	sti
	iret


#
# INTEL_WRAPPER
# Description:  provides the assembly linkage to safely
# 			    call the Intel reserved exception handler
# No inputs and outputs
#
INTEL_WRAPPER:
	cli
	pushal
	pushfl
	call interrupt_intel
	popfl
	popal
	sti
	iret


#
# WRAPPER_[0-19]
# Description:  provides the assembly linkage to safely
# 			    call the individual kernel-defined 
#				exception handlers
# No inputs and outputs
#
WRAPPER_0:
	cli
	pushal
	pushfl
	call interrupt_0
	popfl
	popal
	sti
	iret


WRAPPER_1:
	cli
	pushal
	pushfl
	call interrupt_1
	popfl
	popal
	sti
	iret


WRAPPER_2:
	cli
	pushal
	pushfl
	call interrupt_2
	popfl
	popal
	sti
	iret


WRAPPER_3:
	cli
	pushal
	pushfl
	call interrupt_3
	popfl
	popal
	sti
	iret


WRAPPER_4:
	cli
	pushal
	pushfl
	call interrupt_4
	popfl
	popal
	sti
	iret


WRAPPER_5:
	cli
	pushal
	pushfl
	call interrupt_5
	popfl
	popal
	sti
	iret


WRAPPER_6:
	cli
	pushal
	pushfl
	call interrupt_6
	popfl
	popal
	sti
	iret


WRAPPER_7:
	cli
	pushal
	pushfl
	call interrupt_7
	popfl
	popal
	sti
	iret


WRAPPER_8:
	cli
	pushal
	pushfl
	call interrupt_8
	popfl
	popal
	sti
	iret


WRAPPER_9:
	cli
	pushal
	pushfl
	call interrupt_9
	popfl
	popal
	sti
	iret


WRAPPER_10:
	cli
	pushal
	pushfl
	call interrupt_10
	popfl
	popal
	sti
	iret


WRAPPER_11:
	cli
	pushal
	pushfl
	call interrupt_11
	popfl
	popal
	sti
	iret


WRAPPER_12:
	cli
	pushal
	pushfl
	call interrupt_12
	popfl
	popal
	sti
	iret


WRAPPER_13:
	cli
	pushal
	pushfl
	call interrupt_13
	popfl
	popal
	sti
	iret


WRAPPER_14:
	cli
	pushal
	pushfl
	call interrupt_14
	popfl
	popal
	sti
	iret


WRAPPER_15:
	cli
	pushal
	pushfl
	call interrupt_15
	popfl
	popal
	sti
	iret


WRAPPER_16:
	cli
	pushal
	pushfl
	call interrupt_16
	popfl
	popal
	sti
	iret


WRAPPER_17:
	cli
	pushal
	pushfl
	call interrupt_17
	popfl
	popal
	sti
	iret


WRAPPER_18:
	cli
	pushal
	pushfl
	call interrupt_18
	popfl
	popal
	sti
	iret


WRAPPER_19:
	cli
	pushal
	pushfl
	call interrupt_19
	popfl
	popal
	sti
	iret


SYS_CALL_WRAPPER:
	cli
	pushfl

SYS_CALL_ONE:	
	cmp $1, %eax
	jne SYS_CALL_TWO
	push %ebx
	call halt
	pop %ebx
	jmp END_SYS_CALL_WRAPPER

SYS_CALL_TWO:	
	cmp $2, %eax
	jne SYS_CALL_THREE
	push %ebx
	call execute
	pop %ebx
	jmp END_SYS_CALL_WRAPPER

SYS_CALL_THREE:	
	cmp $3, %eax
	jne SYS_CALL_FOUR
	push %edx
	push %ecx
	push %ebx
	call read
	pop %ebx
	pop %ecx
	pop %edx
	jmp END_SYS_CALL_WRAPPER

SYS_CALL_FOUR:	
	cmp $4, %eax
	jne SYS_CALL_FIVE
	push %edx
	push %ecx
	push %ebx
	
	call write

	pop %ebx
	pop %ecx
	pop %edx
	jmp END_SYS_CALL_WRAPPER

SYS_CALL_FIVE:	
	cmp $5, %eax
	jne SYS_CALL_SIX
	push %ebx
	call open
	pop %ebx
	jmp END_SYS_CALL_WRAPPER

SYS_CALL_SIX:	
	cmp $6, %eax
	jne SYS_CALL_SEVEN
	push %ebx
	call close
	pop %ebx
	jmp END_SYS_CALL_WRAPPER

SYS_CALL_SEVEN:	
	cmp $7, %eax
	jne SYS_CALL_EIGHT
	push %ecx
	push %ebx
	call getargs
	pop %ebx
	pop %ecx
	jmp END_SYS_CALL_WRAPPER

SYS_CALL_EIGHT:	
	cmp $8, %eax
	jne SYS_CALL_NINE
	push %ebx
	call vidmap
	pop %ebx
	jmp END_SYS_CALL_WRAPPER

SYS_CALL_NINE:
SYS_CALL_TEN:
	movl $-1, %eax



END_SYS_CALL_WRAPPER:
	popfl
	sti
	iret

