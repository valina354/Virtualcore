#define PROMPT1_ADDR 1000       ; Address for "Enter first number: " string
#define PROMPT2_ADDR 1050       ; Address for "Enter operator (+ - * /): " string
#define PROMPT3_ADDR 1100       ; Address for "Enter second number: " string
#define RESULT_MSG_ADDR 1150    ; Address for "Result: " string
#define ERR_OP_MSG_ADDR 1200    ; Address for "Invalid operator!" string

#define INPUT1_BUF_ADDR 2000    ; Buffer for first number string input
#define INPUT2_BUF_ADDR 2050    ; Buffer for second number string input
#define OP_BUF_ADDR 2100        ; Buffer for operator string input
#define INPUT_BUF_LEN 20        ; Max length for number/operator strings + null

#define ASCII_PLUS 43
#define ASCII_MINUS 45
#define ASCII_MULTIPLY 42
#define ASCII_DIVIDE 47
#define ASCII_ZERO 48
#define CONST_NINE 9            ; Constant 9 for digit check
#define CONST_ZERO 0            ; Constant 0 for checks
#define CONST_ONE 1             ; Constant 1 for negative flag

_start:
    STRMOV PROMPT1_ADDR, "Enter first number: "
    STRMOV PROMPT2_ADDR, "Enter operator (+ - * /): "
    STRMOV PROMPT3_ADDR, "Enter second number: "
    STRMOV RESULT_MSG_ADDR, "Result: "
    STRMOV ERR_OP_MSG_ADDR, "Invalid operator!"

    ; --- Get First Number ---
    MOV R0, PROMPT1_ADDR
    INT 0x02                ; Print prompt
    MOV R0, INPUT1_BUF_ADDR
    MOV R1, INPUT_BUF_LEN
    INT 0x05                ; Get string input

    ; --- Convert First Number String to Integer ---
    MOV R10, INPUT1_BUF_ADDR ; Pass string address in R10
    CALL _string_to_int     ; Result in R0
    MOV R2, R0              ; Store first integer in R2

    ; --- Get Operator ---
    MOV R0, PROMPT2_ADDR
    INT 0x02
    MOV R0, OP_BUF_ADDR
    MOV R1, 3               ; Buffer size for operator char + potential newline + null
    INT 0x05
    MOV R10, OP_BUF_ADDR
    LOAD R4, R10            ; Load the operator character into R4

    ; --- Get Second Number ---
    MOV R0, PROMPT3_ADDR
    INT 0x02
    MOV R0, INPUT2_BUF_ADDR
    MOV R1, INPUT_BUF_LEN
    INT 0x05

    ; --- Convert Second Number String to Integer ---
    MOV R10, INPUT2_BUF_ADDR ; Pass string address in R10
    CALL _string_to_int     ; Result in R0
    MOV R3, R0              ; Store second integer in R3

    ; --- Perform Calculation Based on Operator ---
    MOV R5, ASCII_PLUS      ; Check for '+'
    CMP R4, R5
    JEQ _add

    MOV R5, ASCII_MINUS     ; Check for '-'
    CMP R4, R5
    JEQ _subtract

    MOV R5, ASCII_MULTIPLY  ; Check for '*'
    CMP R4, R5
    JEQ _multiply

    MOV R5, ASCII_DIVIDE    ; Check for '/'
    CMP R4, R5
    JEQ _divide

    ; --- Invalid Operator ---
    MOV R0, ERR_OP_MSG_ADDR
    INT 0x02
    INT 0x03
    JMP _end_program

    ; --- Calculation Blocks ---
_add:
    ADD R2, R3
    JMP _print_result

_subtract:
    SUB R2, R3
    JMP _print_result

_multiply:
    MUL R2, R3
    JMP _print_result

_divide:
    MOV R5, CONST_ZERO      ; Check for divide by zero
    CMP R3, R5
    JNE _do_divide
    STRMOV INPUT1_BUF_ADDR, "Error: Division by zero!"
    MOV R0, INPUT1_BUF_ADDR
    INT 0x02
    INT 0x03
    JMP _end_program
_do_divide:
    DIV R2, R3
    JMP _print_result

    ; --- Print the Result ---
_print_result:
    MOV R0, RESULT_MSG_ADDR
    INT 0x02                ; Print "Result: "
    MOV R0, R2
    INT 0x01                ; Print integer result
    INT 0x03

    ; --- End Program ---
_end_program:
    HLT

; --- Subroutine: _string_to_int ---
; Converts a null-terminated string of digits (optional leading '-') at memory address in R10
; into an integer value, returned in R0. Handles non-digits by stopping conversion.
; Uses registers: R0 (result), R5 (current char), R6 (digit value), R7 (temp for *10),
;                 R8 (holds ASCII_ZERO), R9 (holds CONST_ZERO), R11 (holds CONST_NINE)
;                 R10 (pointer), R12 (negative_flag), R13 (ASCII_MINUS)
_string_to_int:
    MOV R0, CONST_ZERO      ; Initialize result
    MOV R8, ASCII_ZERO      ; Load constant '0' (48)
    MOV R9, CONST_ZERO      ; Load constant 0
    MOV R11, CONST_NINE     ; Load constant 9
    MOV R12, CONST_ZERO     ; R12 = negative_flag, initialize to 0 (positive)
    MOV R13, ASCII_MINUS    ; Load constant '-' (45)

    ; Check for leading minus sign
    LOAD R5, R10            ; Load the first character
    CMP R5, R13             ; Compare first char with '-'
    JNE _s2i_start_loop     ; If not '-', start the main loop

    ; It is a '-', handle it
    MOV R12, CONST_ONE      ; Set negative_flag (R12 = 1)
    INC R10                 ; Advance pointer past the '-' sign

    ; Start of the main conversion loop (label for jumping after handling '-')
_s2i_start_loop:
    LOAD R5, R10            ; Load current character (either first digit or char after '-')
_s2i_check_char:            ; Label to jump back to after processing a digit
    CMP R5, R9              ; Check for null terminator (0)
    JEQ _s2i_apply_sign     ; If null, finish up

    ; Validate if character is a digit '0' through '9'
    MOV R6, R5
    SUB R6, R8              ; Convert ASCII char to numeric value (char - '0')
    CMP R6, R9              ; Check if < 0
    JMPL _s2i_invalid
    CMP R6, R11             ; Check if > 9
    JMPH _s2i_invalid

    ; Process valid digit
    ; Multiply current result (R0) by 10 using shifts: R0 = (R0 * 8) + (R0 * 2)
    MOV R7, R0
    SHL R7, 1               ; R7 = R0 * 2
    SHL R0, 3               ; R0 = R0 * 8
    ADD R0, R7              ; R0 = (R0*8)+(R0*2) = R0*10

    ADD R0, R6              ; Add the new digit's value

    INC R10                 ; Move to next character in string
    LOAD R5, R10            ; Load the next character
    JMP _s2i_check_char     ; Jump back to check the character

_s2i_invalid:
    ; Stop conversion if non-digit encountered
    JMP _s2i_apply_sign

_s2i_apply_sign:            ; Apply negation if needed before returning
    CMP R12, R9             ; Check negative_flag (is R12 == 0?)
    JEQ _s2i_done           ; If R12 is 0 (positive), jump straight to return

    NEG R0                  ; If R12 was 1 (negative), negate the result in R0

_s2i_done:
    RET                     ; Return to caller (result is in R0)
