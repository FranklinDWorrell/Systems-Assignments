;--------------------------------------------------------------------------------------------------
; Encryption Project 
; CSCI 2450-601 
; 
; Takes a user-provided input file, encrypts it with a user provided key, then outputs the ciper 
; text into a file of the user's chosing. Asks the user if she would like to run the program again 
; or exit after encryption is complete. 
; 
; Author: Franklin D. Worrell 
; Date Due: April 5, 2016 
; Revision Date: April 5, 2016 
;--------------------------------------------------------------------------------------------------

INCLUDE Irvine32.inc 

MAX_FILE_PATH = 260 
MAX_KEY_LENGTH = 12 
BUFFER_SIZE = 16 

.data 
; Strings for communicating with the user. 
sInputPrompt BYTE "Please enter the name of the file you'd like to encode/decode > ", 0 
sNameTooLong BYTE "File name/path exceeds Windows maximum of 260 characters. Please try again.", 0 
sInvalidInput BYTE "Invalid file name/path. Please try again.", 0 
sOutputPrompt BYTE "Please enter the desired name of the output file > ", 0 
sInvalidOutput BYTE "Error creating output file. Please try again with a different file name.", 0 
sKeyPrompt BYTE "Please enter a password of up to 12 characters > ", 0 
sInvalidKey BYTE "Key must be 1-12 characters in length. Please try again.", 0 
sDoneMessage BYTE " successfully XORed with your key into ", 0 
sRepeatPrompt BYTE "Do you wish to run the program again? (y/n) > ", 0 
sBadRepeat BYTE "Please enter only y or n.", 0 
sExitMessage BYTE "Thank, you! Exiting...", 0 

; Global variables for use in file i/o. 
buffer BYTE BUFFER_SIZE DUP(?) 
clearBuffer BYTE BUFFER_SIZE DUP(0) 		; For use in clearing buffer between loops. 
sInputName BYTE MAX_FILE_PATH + 2 DUP(?) 	; Space for file names must be larger than 
sOutputName BYTE MAX_FILE_PATH + 2 DUP(?) 	; 	valid input, so that they can still be 
inputHandle DWORD ? 						; 	validated without overwriting 
outputHandle DWORD ? 						; 	subsequent memory. 
bytesRead DWORD 16 

; Global variables for storing key and for use in control flow. 
key BYTE MAX_KEY_LENGTH + 2 DUP(?) 			; Space for key must be larger than valid input. 
keyLength BYTE 0 							; Otherwise, keylength,etc. will be overwritten. 
lastKeyIndex BYTE 0 						; For use in looping through key during encryption. 
shouldRepeat BYTE 1 						; For use in loop in main procedure. 


.code 
main PROC 
beginRepeatProgramLoop: 
	MOV bytesRead, 16 				; Reset values possibly changed by previous runs 
	MOV lastKeyIndex, 0 			; 	of the encryption procedure. 
	CALL GetInputFile 				; Get a handle for the file being processed. 
	CALL GetOutputFile 				; Get a handle for the output file. 
	CALL GetKey 					; Get a key for encryption/decryption. 
	CALL EncryptText 				; Encrypt the text. 
	CALL CloseBothFiles 			; Close the input and output files. 
	CALL GetRepeatPreference 		; Ask user if she wants to encrypt/decrypt again. 
	MOVZX EAX, shouldRepeat 		; Move loop sentinel value into EAX for comparison. 
	CMP EAX, 1 						; If user entered "y", continue looping. 
	JE beginRepeatProgramLoop 
	
exitProgramLoop: 					; User wished to exit. 
	MOV EDX, OFFSET sExitMessage 	; Display exiting message. 
	CALL WriteString 
	EXIT 
main ENDP 


;--------------------------------------------------------------------------------------------
EncryptText PROC 
; Loops through input file copying 16 bytes at a time to the buffer. After buffer is filled, 
; Calls method that XORs the buffer with the key. After buffer is XORed, the contents of the 
; buffer are written to the output file. 
; Receives: nothing 
; Returns: nothing 
;--------------------------------------------------------------------------------------------
whileBytesLeftToRead: 
	MOV EAX, bytesRead 				; If the number of bytes read is less than the size of 
	CMP EAX, BUFFER_SIZE 			; 	the buffer, then the end of the file was reached. 
	JB fileFinished 
	
	; Read 16 bytes from input file into buffer. 
	MOV EAX, inputHandle 			; Provide ReadFromFile with file handle. 
	MOV EDX, OFFSET buffer 			; Provide ReadFromFile with buffer address. 
	MOV ECX, BUFFER_SIZE 			; Provide ReadFromFile with number or bytes to write. 
	CALL ReadFromFile 				; Call Irvine's read procedure. 
	
	; Check Carry Flag to get number of bytes read or report error. 
	JC reportError 
	MOV bytesRead, EAX 				; Store the bytes read for looping. 
	
	CALL XORBufferKey 				; XOR bytes of buffer with bytes of key. 
	
	; Write 16 bytes from buffer into output file. 
	MOV EAX, outputHandle 			; Provide WriteToFile with file handle. 
	MOV EDX, OFFSET buffer 			; Provide WriteToFile with buffer address. 
	MOV ECX, bytesRead 				; Move the number of bytes read into ECX for WriteToFile. 
	CALL WriteToFile 
	
	; Perform error checking on call to WriteToFile 
	CMP EAX, 0 
	JB reportError 
	
continueLooping: 
	JMP whileBytesLeftToRead 		; Loop until end of file reached . 
	
reportError: 						; If an error in I/O occurred, then print 
	CALL WriteWindowsMsg 			; 	the error's system message. 
	
fileFinished: 
	RET 
EncryptText ENDP 


;--------------------------------------------------------------------------------------------
XORBufferKey PROC 
; XORs the buffer with the key, byte-by-byte. Loops through buffer in outer loop; loops 
; through the key in a nested loop. Instead of using modular arithmetic to keep track of 
; the appropriate index of the key, this implementation begins the index at zero, increments 
; it with each repetition of the loop until it reaches the length of the key, then zeroes it 
; out and continues looping. This implementation seemed to make keeping track of where you 
; left off in the key more straightfoward--though it no doubt required more code. 
; Receives: nothing 
; Returns: nothing 
;--------------------------------------------------------------------------------------------
	MOV ESI, 0 						; ESI will store index of buffer. 
	MOVZX EAX, keyLength 			; Move keyLength for comparison 

whileStillXoring: 					; Main encryption loop. 
	CMP ESI, BUFFER_SIZE 			; If buffer fully read, return procedure. 
	JAE doneXORingBuffer 
	
	
	; Iterate through the key so that it cycles as bytes continue being read. 
	loopThroughKey: 
		XOR EDX, EDX 				; Zero-out EDX. 
		CMP AL, lastKeyIndex 		; If all of key not used, continue from previous index. 
		JA keyNotSpent 
		MOV lastKeyIndex, 0 		; Otherwise, reset key index to zero to continue loop. 
	
	keyNotSpent: 
		MOVZX EDI, lastKeyIndex 	; Move key index into usable register. 
		MOV DL, [key + EDI] 		; Move current byte of key in order to call XOR. 
		XOR buffer[ESI], DL 		; Encrypt with XOR. 
		INC lastKeyIndex 			; Increment the key index. 
		INC ESI 					; Increment the buffer index. 
		
	JMP whileStillXoring 
	
doneXORingBuffer: 
	RET 
XORBufferKey ENDP 


;--------------------------------------------------------------------------------------------
CloseBothFiles PROC 
; Closes both the input and the output files. 
; Receives: nothing 
; Returns: nothing 
;--------------------------------------------------------------------------------------------
	MOV EAX, inputHandle 			; Close the input file. 
	CALL CloseFile 
	MOV EAX, outputHandle 			; Close the output file. 
	CALL CloseFile 
	RET 
CloseBothFiles ENDP 


;--------------------------------------------------------------------------------------------
GetInputFile PROC 
; Gets the name or path of the input file from the user. Performs validation of length of file 
; name/path as well as validation of the file location and name. 
; Receives: nothing 
; Returns: an input file name stored in inputName, 
; 		   a handle for the input file stored in inputHandle 
;--------------------------------------------------------------------------------------------
	MOV EDX, OFFSET sInputPrompt 	; Prompt user to enter a file name. 
	CALL WriteString 
	
	MOV EDX, OFFSET sInputName 		; Provide ReadString with buffer's address. 
	MOV ECX, MAX_FILE_PATH + 2 		; Provide ReadString with max length for file name. 
	CALL ReadString 				; Irvine's procedure for capturing inputted string. 
	CALL Crlf 						; Move to next line to keep it clean 
	
	CMP EAX, MAX_FILE_PATH 			; Validate entered string for length. 
	JA invalidNameLength 			; Name of file too long. 
	
	MOV EDX, OFFSET sInputName 		; Give OpenInputFile address of filename 
	CALL OpenInputFile 				; Call procedure that returns file handle in EAX 
	CMP EAX, INVALID_HANDLE_VALUE 	; Validate file name. 
	JE handleInvalidInputName 
	MOV inputHandle, EAX 			; Store the valid file handle for use later. 
	JMP returnGetInput 
	
handleInvalidInputName: 
	MOV EDX, OFFSET sInvalidInput 	; Inform user of error. 
	CALL WriteString 
	CALL Crlf 
	CALL GetInputFile 				; Make recursive GetInputFile call so user can try again. 
	JMP returnGetInput 
	
invalidNameLength: 
	MOV EDX, OFFSET sNameTooLong 	; Inform user that file name exceeds maximum. 
	CALL WriteString 
	CALL Crlf 
	CALL GetInputFile 				; Make recursive call so user can try again. 
	JMP returnGetInput 
	
returnGetInput: 
	RET 
GetInputFile ENDP 


;--------------------------------------------------------------------------------------------
GetOutputFile PROC 
; Gets the name of the desired output file from the user. Performs validation of length of 
; file name/path as well as validation of the file location and name. 
; Receives: nothing 
; Returns: output file's name stored in sOutputName, 
; 		   a handle for the output file stored in outputHandle 
;--------------------------------------------------------------------------------------------
	MOV EDX, OFFSET sOutputPrompt 	; Prompt the user for a file name. 
	CALL WriteString 
	
	MOV EDX, OFFSET sOutputName 	; Provide ReadString with buffer's address. 
	MOV ECX, MAX_FILE_PATH + 2 		; Provide ReadString with max length for file name. 
	CALL ReadString 				; Irvine's procedure for capturing inputted string. 
	CALL Crlf 						; Move to next line to keep it clean 
	
	CMP EAX, MAX_FILE_PATH 			; Validate entered string for length. 
	JA invalidNameLength 			; Name of file too long. 
	
	MOV EDX, OFFSET sOutputName 	; Give CreateOutputFile address of file name. 
	CALL CreateOutputFile 			; Call procedure that returns file handle in EAX. 
	CMP EAX, INVALID_HANDLE_VALUE 	; Validate file name. 
	JE handleInvalidOutputName 
	MOV outputHandle, EAX 			; Store the valid file handle for use later. 
	JMP returnGetOutput 
	
handleInvalidOutputName: 
	MOV EDX, OFFSET sInvalidOutput 	; Inform user of error. 
	CALL WriteString 
	CALL Crlf 
	CALL GetOutputFile 				; Make recursive GetOutputFile call so user can try again. 
	JMP returnGetOutput 
	
invalidNameLength: 
	MOV EDX, OFFSET sNameTooLong 	; Inform user that file name exceeds maximum. 
	CALL WriteString 
	CALL Crlf 
	CALL GetOutputFile 				; Make recursive call so user can try again. 
	JMP returnGetOutput 
	
returnGetOutput: 
	RET 
GetOutputFile ENDP 


;--------------------------------------------------------------------------------------------
GetKey PROC 
; Asks the user to provide a key, validates the provided key, and stores key and length of key. 
; Receives: nothing 
; Returns: length of key stored in keyLength, 
; 		   key stored in key 
;--------------------------------------------------------------------------------------------
	MOV EDX, OFFSET sKeyPrompt 		; Prompt user to provide a key. 
	CALL WriteString 
	MOV EDX, OFFSET key 			; Provide ReadString with buffer for key. 
	MOV ECX, MAX_KEY_LENGTH + 2 	; Max string size must be > 13 for input validation. 
	CALL ReadString 
	
	; Validate the user input: 0 < key length < 13. 
	CMP EAX, MAX_KEY_LENGTH 		; Key cannot be more than 12 characters long 
	JA handleInvalidKeyInput 
	CMP EAX, 0 						; Key cannot be 0 characters long. 
	JE handleInvalidKeyInput 
	JMP keyInputValid 
	
handleInvalidKeyInput: 				; User entered an invalid key. 
	CALL Crlf 
	MOV EDX, OFFSET sInvalidKey 	; Inform user of invalid input 
	CALL WriteString 
	CALL Crlf 
	CALL GetKey 					; Allow user to try entering a key again. 
	
keyInputValid: 						; User entered a valid key. 
	MOV keyLength, AL 				; Store length of valid key. 
	CALL Crlf 
	RET 
GetKey ENDP 


;--------------------------------------------------------------------------------------------
GetRepeatPreference PROC 
; Asks the user whether she would like to repeat the encryption/decryption process. 
; Validates user input to ensure that only 'y' or 'n' are accepted as answers. 
; Receives: nothing 
; Returns: shouldRepeat == 1 if user wishes to continue, 0 otherwise 
;--------------------------------------------------------------------------------------------
	MOV EDX, OFFSET sInputName 		; Display a message tellling user encryption finished. 
	CALL WriteString 				; Entire message must be built from three different 
	MOV EDX, OFFSET sDoneMessage 	; 	saved strings, so this takes a bit of code. 
	CALL WriteString 
	MOV EDX, OFFSET sOutputName 
	CALL WriteString 
	CALL Crlf 
	
	MOV EDX, OFFSET sRepeatPrompt 	; Display prompt asking user if she wants to run again. 
	CALL WriteString 
	CALL ReadChar 					; Get preference from user. 
	CALL Crlf 
	
	CMP al, 'y' 					; User wishes to continue. 
	JE returnGetRepeatPreference 	; So, shouldRepeat is not changed from default value. 
	CMP al, 'n' 					; User wishes to quit. 
	JE userDeclined 
	
	; Otherwise, user entered invalid input, so inform them, then repeat request. 
	MOV EDX, OFFSET sBadRepeat 		; Display invalid input message. 
	CALL WriteString 
	CALL Crlf 
	CALL GetRepeatPreference 		; Make recursive call until user provides valid input. 
	JMP returnGetRepeatPreference 
	
userDeclined: 						; User wished to exit. 
	MOV shouldRepeat, 0 			; shouldRepeat is set to zero. 
	
returnGetRepeatPreference: 
	RET 
GetRepeatPreference ENDP 
END main