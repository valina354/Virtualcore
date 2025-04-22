#define MEMSET_TARGET_ADDR 100      ; Where MEMSET will write
#define MEMCPY_SOURCE_ADDR 200      ; Source data for MEMCPY
#define MEMCPY_DEST_ADDR   300      ; Destination for MEMCPY
#define TEST_LENGTH        16       ; How many bytes to set/copy
#define FILL_BYTE          0xAA     ; Value to use for MEMSET

#define MSG_START           500     ; Addresses for messages
#define MSG_MEMSET_RUN      550
#define MSG_MEMSET_DUMP     600
#define MSG_MEMCPY_PREP     650
#define MSG_MEMCPY_RUN      700
#define MSG_MEMCPY_DUMP_SRC 750
#define MSG_MEMCPY_DUMP_DST 800
#define MSG_COMPARE_RUN     850
#define MSG_COMPARE_SUCCESS 900
#define MSG_COMPARE_FAIL    950
#define MSG_END            1000

; --- Data Area - Strings ---
; Make sure these are placed *after* the HLT or jumped over
STRMOV MSG_START,           "--- MEMSET/MEMCPY Test Start ---"
STRMOV MSG_MEMSET_RUN,      "Running MEMSET..."
STRMOV MSG_MEMSET_DUMP,     "Dumping MEMSET region (Should be AA AA ...):"
STRMOV MSG_MEMCPY_PREP,     "Preparing MEMCPY source..."
STRMOV MSG_MEMCPY_RUN,      "Running MEMCPY..."
STRMOV MSG_MEMCPY_DUMP_SRC, "Dumping MEMCPY source region:"
STRMOV MSG_MEMCPY_DUMP_DST, "Dumping MEMCPY destination region (Should match source):"
STRMOV MSG_COMPARE_RUN,     "Comparing MEMCPY destination against source..."
STRMOV MSG_COMPARE_SUCCESS, "SUCCESS: Copied data matches source!"
STRMOV MSG_COMPARE_FAIL,    "FAILURE: Copied data MISMATCH!"
STRMOV MSG_END,             "--- Test End ---"

; --- Program Start ---
main:
    MOV R0, MSG_START
    INT 0x02                ; Print "--- Test Start ---"

; --- MEMSET Test ---
    MOV R0, MSG_MEMSET_RUN
    INT 0x02                ; Print "Running MEMSET..."

    MOV R1, MEMSET_TARGET_ADDR ; R1 = Destination address
    MOV R2, FILL_BYTE          ; R2 = Value to set (uses lower 8 bits)
    MOV R3, TEST_LENGTH        ; R3 = Length
    MEMSET R1, R2, R3          ; Execute MEMSET

    MOV R0, MSG_MEMSET_DUMP
    INT 0x02                ; Print "Dumping MEMSET region..."
    MOV R0, MEMSET_TARGET_ADDR ; Dump Address
    MOV R1, TEST_LENGTH        ; Dump Length
    INT 0x22                   ; Dump Memory

; --- MEMCPY Test ---
    MOV R0, MSG_MEMCPY_PREP
    INT 0x02                ; Print "Preparing MEMCPY source..."
    ; Put known data into the source location. Make sure length <= TEST_LENGTH
    STRMOV MEMCPY_SOURCE_ADDR, "SourceDataTest!!" ; 16 bytes exactly

    MOV R0, MSG_MEMCPY_RUN
    INT 0x02                ; Print "Running MEMCPY..."

    MOV R1, MEMCPY_DEST_ADDR   ; R1 = Destination address
    MOV R2, MEMCPY_SOURCE_ADDR ; R2 = Source address
    MOV R3, TEST_LENGTH        ; R3 = Length
    MEMCPY R1, R2, R3          ; Execute MEMCPY

    MOV R0, MSG_MEMCPY_DUMP_SRC
    INT 0x02                ; Print "Dumping MEMCPY source..."
    MOV R0, MEMCPY_SOURCE_ADDR
    MOV R1, TEST_LENGTH
    INT 0x22                   ; Dump Source Memory

    MOV R0, MSG_MEMCPY_DUMP_DST
    INT 0x02                ; Print "Dumping MEMCPY destination..."
    MOV R0, MEMCPY_DEST_ADDR
    MOV R1, TEST_LENGTH
    INT 0x22                   ; Dump Destination Memory

; --- Verify MEMCPY using a loop ---
    MOV R0, MSG_COMPARE_RUN
    INT 0x02                ; Print "Comparing copy..."

    MOV R4, MEMCPY_SOURCE_ADDR ; R4 = Pointer to source
    MOV R5, MEMCPY_DEST_ADDR   ; R5 = Pointer to destination
    MOV R6, TEST_LENGTH        ; R6 = Counter for loop

verify_loop:
    LOAD R7, R4             ; Load byte from source addr in R4
    LOAD R8, R5             ; Load byte from dest addr in R5
    CMP R7, R8              ; Compare the bytes
    JNE verify_fail         ; Jump to fail label if Not Equal

    INC R4                  ; Increment source pointer
    INC R5                  ; Increment destination pointer
    ; R6 is automatically decremented by LOOP
    LOOP R6, verify_loop    ; Decrement R6, if R6 != 0, jump to verify_loop

    ; If we fall through the loop, it means all bytes matched
    MOV R0, MSG_COMPARE_SUCCESS
    INT 0x02                ; Print "SUCCESS..."
    JMP end_test            ; Jump to the end

verify_fail:
    MOV R0, MSG_COMPARE_FAIL
    INT 0x02                ; Print "FAILURE..."

end_test:
    MOV R0, MSG_END
    INT 0x02                ; Print "--- Test End ---"
    HLT                     ; Halt the CPU
