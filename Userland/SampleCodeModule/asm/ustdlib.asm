GLOBAL _syscall
GLOBAL _opcodeExp
GLOBAL _hlt
section .text


%macro pushState 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

_syscall:
	push rbp
	mov rbp, rsp
	pushState

    int 80h

	popState
	mov rsp, rbp
	pop rbp

	ret

_opcodeExp:
	UD2
	ret
