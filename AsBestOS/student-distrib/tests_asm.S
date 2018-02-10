.globl syscalls_test_read, syscalls_test_write, syscalls_test_close, syscalls_test_open

syscalls_test_read:
    PUSHL	%EBX
	MOVL	$3,%EAX
	MOVL	8(%ESP),%EBX
	MOVL	12(%ESP),%ECX
	MOVL	16(%ESP),%EDX
	INT	$0x80
	CMP	$0xFFFFC000,%EAX
	JBE	sctr1
	MOVL	$-1,%EAX
sctr1:
	POPL	%EBX
	RET

syscalls_test_write:
    PUSHL	%EBX
	MOVL	$4,%EAX
	MOVL	8(%ESP),%EBX
	MOVL	12(%ESP),%ECX
	MOVL	16(%ESP),%EDX
	INT	$0x80
	CMP	$0xFFFFC000,%EAX
	JBE	sctw1
	MOVL	$-1,%EAX
sctw1:
	POPL	%EBX
	RET

syscalls_test_close:
    PUSHL	%EBX
	MOVL	$6,%EAX
	MOVL	8(%ESP),%EBX
	MOVL	12(%ESP),%ECX
	MOVL	16(%ESP),%EDX
	INT	$0x80
	CMP	$0xFFFFC000,%EAX
	JBE	sctr1
	MOVL	$-1,%EAX
sctc1:
	POPL	%EBX
	RET

syscalls_test_open:
    PUSHL	%EBX
	MOVL	$5,%EAX
	MOVL	8(%ESP),%EBX
	MOVL	12(%ESP),%ECX
	MOVL	16(%ESP),%EDX
	INT	$0x80
	CMP	$0xFFFFC000,%EAX
	JBE	scto1
	MOVL	$-1,%EAX
scto1:
	POPL	%EBX
	RET


