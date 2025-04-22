; program.asm - Test Graphics Drawing

; --- Initialization ---
MOV R0, 1      ; Value for screen on
INT 0x16       ; INT_SCREEN_ON: Enable screen updates

MOV R0, 0      ; Palette index 0 (Black)
INT 0x15       ; INT_CLEAR_GFX_SCREEN: Clear screen

; --- Draw Horizontal Line ---
; Draw a white line (color 15) at Y=96 from X=10 to X=245
MOV R1, 96       ; Y coordinate
MOV R2, 15       ; Color index 15 (White)
MOV R0, 10       ; Start X coordinate
MOV R3, 246      ; End X coordinate + 1

h_line_loop:
  INT 0x14       ; INT_DRAW_PIXEL
  INC R0
  CMP R0, R3
  JMPL h_line_loop ; Loop if R0 < R3

; --- Draw Vertical Line ---
; Draw a red line (color 4) at X=128 from Y=10 to Y=181
MOV R0, 128      ; X coordinate
MOV R2, 4        ; Color index 4 (Red)
MOV R1, 10       ; Start Y coordinate
MOV R3, 182      ; End Y coordinate + 1

v_line_loop:
  INT 0x14       ; INT_DRAW_PIXEL
  INC R1
  CMP R1, R3
  JMPL v_line_loop ; Loop if R1 < R3

; --- Draw Filled Blue Box ---
; Draw a blue box (color 1) from (30, 30) to (59, 59)
MOV R2, 1        ; Color index 1 (Blue)
MOV R4, 60       ; Box End Y + 1
MOV R5, 60       ; Box End X + 1

MOV R1, 30       ; Start Y coordinate (outer loop)
box_outer_loop:
  CMP R1, R4     ; Check if Y loop finished
  JEQ box_done

  MOV R0, 30     ; Start X coordinate for this row (inner loop)
  box_inner_loop:
    CMP R0, R5   ; Check if X loop finished for this row
    JEQ box_inner_loop_done

    INT 0x14     ; INT_DRAW_PIXEL
    INC R0
    JMP box_inner_loop ; Continue inner loop

  box_inner_loop_done:
    INC R1       ; Move to next row
    JMP box_outer_loop ; Continue outer loop

box_done:
  ; Box drawing finished

; --- Draw Random Pixels ---
; Draw 100 random pixels with random colors
MOV R6, 100      ; Pixel count
MOV R7, 256      ; Screen width (for MOD)
MOV R8, 192      ; Screen height (for MOD)
MOV R9, 16       ; Palette size (for MOD)
MOV R10, 0       ; Loop termination value

random_loop:
  RND R0         ; Random number for X
  MOD R0, R7     ; X = random % width
  RND R1         ; Random number for Y
  MOD R1, R8     ; Y = random % height
  RND R2         ; Random number for color index
  MOD R2, R9     ; Color = random % palette_size

  INT 0x14       ; INT_DRAW_PIXEL

  DEC R6         ; Decrement pixel counter
  CMP R6, R10    ; Compare counter with 0
  JMPH random_loop ; Loop if counter > 0


; --- Pause and Halt ---
MOV R0, 1000     ; Sleep for 1000 milliseconds (1 second)
INT 0x04         ; INT_SLEEP_MS

HLT              ; Halt execution
