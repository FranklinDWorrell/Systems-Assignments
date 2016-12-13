; Fibonacci.asm - Computes and prints the third Fibonacci number.
; Homework 3, CSCI 2450-601
;
; Author: Franklin D. Worrell
; Date Due: March 29, 2016
; Revision Date: March 28, 2016

INCLUDE Irvine32.inc
.386
.stack 4096
ExitProcess proto,dwExitCode:dword

.code
main PROC
	XOR		EAX, EAX			; Zero-out EAX
	XOR		EBX, EBX			; Zero-out EBX
	XOR		EDX, EDX			; Zero-out EDX
	PUSH	3					; Push argument for Fibonacci procedure to stack	
	CALL	Fibonacci			; Call Fibonacci procedure
	CALL	WriteDec			; Print the 3rd Fibonacci number
	CALL	Crlf				; Write end of line sequence to console window
	invoke ExitProcess,0
main ENDP

;--------------------------------------------------------------------------------
Fibonacci PROC 
; Calculates the nth Fibonacci argument. 
; Fibonacci(n) = Fibonacci(n - 1) + Fibonacci(n -2)
; Makes recursive calls until a base case--n = 1 or n = 0--is reached. 
; Receives:		[EBP + 8] = n, the Fibonacci number to calculate
; Returns:		EAX = the nth Fibonacci number
;--------------------------------------------------------------------------------

	PUSH	EBP
	MOV		EBP, ESP

	SUB		ESP, 8						; Create space for 2 integers
	MOV		EBX, [EBP + 8]				; Retrieve n from the stack, EBX = n

	; Push n - 1 and n -2 to the stack for use in recursive calls.
	MOV		EDX, EBX					; EDX = n
	DEC		EDX							; EDX = n - 1
	MOV		DWORD PTR [EBP - 4], EDX	; Store n - 1 on the stack
	DEC		EDX							; EDX = n - 2
	MOV		DWORD PTR [EBP - 8], EDX	; Store n - 2 on the stack

	CMP		EBX, 1						; If n > 1, make recursive calls.
	JA		ComplexCase 

	CMP		EBX, 1						; If n = 1, base case reached.
	JE		NEqualsOne

	CMP		EBX, 0						; If n = 0, base case reached. 
	JBE		NEqualsZero

	; n > 1, so recursive calls made to reduce to base cases. 
	ComplexCase: 
			MOV		EBX, [EBP - 4]		; Get n - 1 from where it was pushed to stack
			PUSH	EBX					; Store n - 1 on the stack for recursive call
			CALL	Fibonacci			; Recursive call to calculate Fibonacci(n - 1)

			MOV		EBX, [EBP - 8]		; Get n - 2 from where it was pushed to stack
			PUSH	EBX					; Store n - 2 on the stack for recursive call
			CALL	Fibonacci			; Recursive call to calculate Fibonacci(n - 2) 

			JMP		FibonacciReturn		; After recursive calls, jump to return sequence

	; n = 1, so add Fibonacci(1) to EAX
	NEqualsOne: 
			ADD		EAX, 1				; Fibonacci(1) = 1
			JMP		FibonacciReturn		; Jump to return sequence

	; n = 0, so add Fibonacci(0) to EAX
	NEqualsZero: 
			ADD		EAX, 0				; Fibonacci(0) = 0
			JMP		FibonacciReturn		; Jump to return sequence

	; Instructions for procedure return
	FibonacciReturn: 
			MOV		ESP, EBP			; Clean up the stack--local variables were used. 
			POP		EBP
			RET		

Fibonacci ENDP
END main