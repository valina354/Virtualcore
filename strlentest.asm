#define STRING_ADDR     100       ; Memory address for our first test string
#define EMPTY_STR_ADDR  150       ; Memory address for the empty string
#define MSG_LEN_IS      200       ; Address for "Length is: " message
#define MSG_EMPTY_LEN   250       ; Address for "Empty length is: " message
#define MSG_ZF_OK       300       ; Address for "Zero flag OK.\n" message
#define MSG_ZF_BAD      350       ; Address for "Zero flag BAD.\n" message

; --- Store strings and messages in memory ---
STRMOV STRING_ADDR, "Test!"      ; The string we want the length of (length 5)
STRMOV EMPTY_STR_ADDR, ""         ; An empty string (length 0)
STRMOV MSG_LEN_IS, "Length is: "
STRMOV MSG_EMPTY_LEN, "Empty length is: "
STRMOV MSG_ZF_OK, "Zero flag was set correctly.\n"
STRMOV MSG_ZF_BAD, "Zero flag was NOT set correctly.\n"

; --- Test 1: Get length of "Test!" ---

; Print the "Length is: " message
MOV R0, MSG_LEN_IS
INT 0x02

; Load the address of "Test!" into R1
MOV R1, STRING_ADDR

; Calculate the length using STRLEN. R1 holds the address, R2 will get the length.
STRLEN R2, R1

; Move the calculated length from R2 into R0 for printing
MOV R0, R2

; Print the length (should be 5)
INT 0x01

; Print a newline
INT 0x03

; --- Test 2: Get length of "" (empty string) ---

; Print the "Empty length is: " message
MOV R0, MSG_EMPTY_LEN
INT 0x02

; Load the address of the empty string into R1
MOV R1, EMPTY_STR_ADDR

; Calculate the length. R1 has address, R2 gets length.
; STRLEN should set the Zero Flag here because the length is 0.
STRLEN R2, R1

; Move the length from R2 to R0 for printing
MOV R0, R2

; Print the length (should be 0)
INT 0x01

; Print a newline
INT 0x03

; --- Test 3: Check if Zero Flag was set correctly by the empty string STRLEN ---
JEQ zero_flag_set_correctly  ; If Zero Flag is set (because length was 0), jump

; If we reach here, the Zero Flag was *not* set - this is an error
MOV R0, MSG_ZF_BAD
INT 0x02
JMP end_program            ; Jump to the end

zero_flag_set_correctly:
; Zero Flag was set as expected
MOV R0, MSG_ZF_OK
INT 0x02

end_program:
; Halt execution
HLT
