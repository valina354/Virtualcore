#define SRC_ADDR    100     ; Address for the source string "Copy Me!"
#define DEST_ADDR   150     ; Address for the destination buffer
#define EMPTY_SRC   200     ; Address for an empty source string ""

#define MSG_BEFORE  250     ; "Dest before: "
#define MSG_AFTER   300     ; "Dest after copy: "
#define MSG_EMPTY   350     ; "Copying empty string..."
#define MSG_AFTER_EMPTY 400 ; "Dest after empty copy: "
#define MSG_END     450     ; "STRCPY test finished.\n"

; --- Store strings and messages in memory ---
STRMOV SRC_ADDR,    "Copy Me!"
STRMOV DEST_ADDR,   "XXXXXXXXXX"  ; Initialize destination with placeholders
STRMOV EMPTY_SRC,   ""

STRMOV MSG_BEFORE,      "Dest before: "
STRMOV MSG_AFTER,       "Dest after copy: "
STRMOV MSG_EMPTY,       "Copying empty string...\n"
STRMOV MSG_AFTER_EMPTY, "Dest after empty copy: "
STRMOV MSG_END,         "STRCPY test finished.\n"

; --- Test 1: Copy "Copy Me!" ---

; Print message and initial destination content
MOV R0, MSG_BEFORE
INT 0x02
MOV R0, DEST_ADDR
INT 0x02              ; Should print "XXXXXXXXXX"
INT 0x03              ; Newline

; Prepare for STRCPY
MOV R1, DEST_ADDR     ; R1 = Destination Address
MOV R2, SRC_ADDR      ; R2 = Source Address

; Perform the copy
STRCPY R1, R2         ; Copy string at R2 (Source) to R1 (Destination)

; Print message and destination content after copy
MOV R0, MSG_AFTER
INT 0x02
MOV R0, DEST_ADDR
INT 0x02              ; Should print "Copy Me!"
INT 0x03              ; Newline
INT 0x03              ; Extra newline for separation

; --- Test 2: Copy empty string ---

; Print message indicating the test
MOV R0, MSG_EMPTY
INT 0x02

; Re-initialize destination to see the change clearly
STRMOV DEST_ADDR,   "YYYYYYYYYY"

; Print message and initial destination content
MOV R0, MSG_BEFORE
INT 0x02
MOV R0, DEST_ADDR
INT 0x02              ; Should print "YYYYYYYYYY"
INT 0x03              ; Newline


; Prepare for STRCPY
MOV R1, DEST_ADDR     ; R1 = Destination Address
MOV R2, EMPTY_SRC     ; R2 = Source Address ("")

; Perform the copy
STRCPY R1, R2         ; Copy string at R2 (Empty) to R1 (Destination)

; Print message and destination content after empty copy
MOV R0, MSG_AFTER_EMPTY
INT 0x02
MOV R0, DEST_ADDR
INT 0x02              ; Should print nothing (an empty line follows)
INT 0x03              ; Newline

; --- End of Tests ---
MOV R0, MSG_END
INT 0x02              ; Print "STRCPY test finished."
HLT                   ; Halt successfully
