#define MSG_START       100
#define MSG_ZF_SET      125
#define MSG_ZF_JUMPED   145
#define MSG_GF_SET      170
#define MSG_GF_JUMPED   220
#define MSG_ZF_CLEARED  250
#define MSG_ZF_NOJUMP   280
#define MSG_GF_STILLSET 315
#define MSG_GF_JUMPED2  355
#define MSG_ALL_CLEARED 390
#define MSG_FINAL_NOJUMP 415
#define MSG_END         460

STRMOV MSG_START,       "Starting flag test.\n"
STRMOV MSG_ZF_SET,      "Zero Flag SET.\n"
STRMOV MSG_ZF_JUMPED,   "JEQ jumped correctly.\n"
STRMOV MSG_GF_SET,      "Greater Flag SET. (ZF should still be SET)\n"
STRMOV MSG_GF_JUMPED,   "JMPH jumped correctly.\n"
STRMOV MSG_ZF_CLEARED,  "Zero Flag CLEARED only.\n"
STRMOV MSG_ZF_NOJUMP,   "JEQ did NOT jump correctly.\n"
STRMOV MSG_GF_STILLSET, "Greater Flag should still be SET.\n"
STRMOV MSG_GF_JUMPED2,  "JMPH jumped correctly again.\n"
STRMOV MSG_ALL_CLEARED, "All flags CLEARED.\n"
STRMOV MSG_FINAL_NOJUMP,"Final jumps did NOT happen correctly.\n"
STRMOV MSG_END,         "Flag test finished.\n"


MOV R0, MSG_START
INT 0x02

CLRF 0             ; Start with all flags clear (Flags = 0x00)

; --- Test 1: Set Zero Flag ---
SETF 0x01          ; Set FLAG_ZERO (Flags = 0x01)
MOV R0, MSG_ZF_SET
INT 0x02
JEQ test1_zf_ok    ; Jump if Zero Flag is set (should jump)
; Error path if JEQ fails
JMP test_failed

test1_zf_ok:
MOV R0, MSG_ZF_JUMPED
INT 0x02

; --- Test 2: Set Greater Flag (without clearing Zero) ---
SETF 0x02          ; Set FLAG_GREATER (Flags = 0x01 | 0x02 = 0x03)
MOV R0, MSG_GF_SET
INT 0x02
JMPH test2_gf_ok   ; Jump if Greater Flag is set (should jump)
; Error path if JMPH fails
JMP test_failed

test2_gf_ok:
MOV R0, MSG_GF_JUMPED
INT 0x02

; --- Test 3: Clear only Zero Flag ---
CLRF 0x01          ; Clear FLAG_ZERO (Flags = 0x03 & ~0x01 = 0x02)
MOV R0, MSG_ZF_CLEARED
INT 0x02
JEQ test_failed    ; Jump if Zero Flag is set (SHOULD NOT jump)

MOV R0, MSG_ZF_NOJUMP
INT 0x02
MOV R0, MSG_GF_STILLSET
INT 0x02
JMPH test3_gf_ok   ; Jump if Greater Flag is set (SHOULD still jump)
; Error path if JMPH fails
JMP test_failed

test3_gf_ok:
MOV R0, MSG_GF_JUMPED2
INT 0x02

; --- Test 4: Clear all flags ---
CLRF 0             ; Clear all defined flags (Flags = 0x00)
MOV R0, MSG_ALL_CLEARED
INT 0x02
JEQ test_failed    ; Jump if Zero Flag is set (should not)
JMPH test_failed   ; Jump if Greater Flag is set (should not)
JMPL test_failed   ; Jump if Less Flag is set (should not)

MOV R0, MSG_FINAL_NOJUMP
INT 0x02

; --- End Test ---
MOV R0, MSG_END
INT 0x02
HLT

test_failed:       ; Common failure point (should not be reached)
MOV R0, 9999        ; Use a distinct value for error output
INT 0x01
HLT
