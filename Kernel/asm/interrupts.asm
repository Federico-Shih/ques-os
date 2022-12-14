
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL endInterrupt
GLOBAL switchRsp
GLOBAL switchContext

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq80Handler
GLOBAL _callTimerTick

GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN scheduleTask
EXTERN exceptionDispatcher
EXTERN syscallHandler
EXTERN getStackBase

SECTION .text

%macro pushState 0
	push rax
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

%macro pushExtraRegisters 0
	push fs
	push gs
%endmacro


%macro popStateNoRax 0
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
	add rsp, 8
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
	pop rax
%endmacro

%macro popExtraRegisters 0
	pop gs
	pop fs
%endmacro


%macro irqHandlerMaster 1
	pushState
	mov rbp, rsp
	
	mov rdi, %1 ; pasaje de parametro
	mov rsi, rsp ; pasa el puntero de rsp
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	call endInterrupt

	popState
	iretq
%endmacro



%macro exceptionHandler 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	mov rsi, rsp
	call exceptionDispatcher

	popState
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	pushState
	pushExtraRegisters

	mov rdi, 0
	mov rsi, rsp
	call irqDispatcher

	mov rdi,rsp
	call scheduleTask
	mov rsp, rax

	call endInterrupt

	popExtraRegisters
	popState
	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never calleduserSample
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

_irq80Handler:
	pushState ;pusheo todos los registros al stack
	call syscallHandler
	popStateNoRax ;popeo todos los registros menos rax para no pisar el valor de retorno de syscall handler
	iretq


;Zero Division Exception
_exception0Handler:
	exceptionHandler 0
;Invalid Opcode Exception
_exception6Handler:
	exceptionHandler 6

endInterrupt:
	push rax
	mov al, 20h
	out 20h, al
	pop rax
	sti
	ret

switchRsp:
	mov rax, rsp
	mov rsp, rdi
	
	sub rsp, 8
	push rbx
	mov rbx, [rax]
	mov [rsp + 8], rbx 
	pop rbx
	ret

; Fuerzo a que se vuelva a la funcion adecuada, para el caso que se mata forzadamente el programa que llamo el SampleCodeModule
switchContext:
	mov rsp, rdi
	popState
	iretq

haltcpu:
	cli
	hlt
	ret

_callTimerTick:
    int 20h
    ret

SECTION .bss
	aux resq 1