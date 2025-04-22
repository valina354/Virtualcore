#define SCREEN_WIDTH    256
#define SCREEN_HEIGHT   192
#define MAX_SNAKE_LEN   100
#define START_LEN       3
#define GAME_SPEED      100

#define SNAKE_X_ARRAY   1000
#define SNAKE_Y_ARRAY   1100

#define REG_HEAD_X      R10
#define REG_HEAD_Y      R11
#define REG_DIRECTION   R12
#define REG_CUR_LEN     R13
#define REG_TEMP1       R14
#define REG_TEMP2       R15
#define REG_ADDR_X      R16
#define REG_ADDR_Y      R17
#define REG_LOOP_IDX    R18

#define BG_COLOR        0
#define SNAKE_COLOR     10
#define HEAD_COLOR      14

#define DIR_RIGHT       0
#define DIR_LEFT        1
#define DIR_UP          2
#define DIR_DOWN        3

#define KEY_RIGHT       79
#define KEY_LEFT        80
#define KEY_DOWN        81
#define KEY_UP          82
#define KEY_ESC         41

_start:
    INT 0x16
    MOV REG_DIRECTION, DIR_RIGHT
    MOV REG_CUR_LEN, START_LEN
    MOV REG_HEAD_X, SCREEN_WIDTH
    MOV REG_TEMP1, 2
    DIV REG_HEAD_X, REG_TEMP1
    MOV REG_HEAD_Y, SCREEN_HEIGHT
    DIV REG_HEAD_Y, REG_TEMP1
    MOV REG_ADDR_X, SNAKE_X_ARRAY
    MOV REG_ADDR_Y, SNAKE_Y_ARRAY
    MOV REG_LOOP_IDX, 0

init_loop:
    CMP REG_LOOP_IDX, REG_CUR_LEN
    JGE init_done
    MOV REG_TEMP1, REG_HEAD_X
    SUB REG_TEMP1, REG_LOOP_IDX
    MOV REG_TEMP2, REG_HEAD_Y
    MOV R0, REG_ADDR_X
    ADD R0, REG_LOOP_IDX
    MOV R1, REG_ADDR_Y
    ADD R1, REG_LOOP_IDX
    STORE REG_TEMP1, R0
    STORE REG_TEMP2, R1
    INC REG_LOOP_IDX
    JMP init_loop

init_done:
game_loop:
    CALL handle_input
    CALL update_snake
    CALL draw_screen
    MOV R0, GAME_SPEED
    INT 0x04
    JMP game_loop

exit_game:
    INT 0x17
    INT 0x35
    HLT

handle_input:
    MOV R0, KEY_ESC
    INT 0x25
    MOV REG_TEMP1, 1
    CMP R0, REG_TEMP1
    JEQ exit_game

    MOV R0, KEY_RIGHT
    INT 0x25
    MOV REG_TEMP1, 1
    CMP R0, REG_TEMP1
    JNE check_left
    MOV REG_TEMP1, DIR_LEFT
    CMP REG_DIRECTION, REG_TEMP1
    JEQ input_done
    MOV REG_DIRECTION, DIR_RIGHT
    JMP input_done

check_left:
    MOV R0, KEY_LEFT
    INT 0x25
    MOV REG_TEMP1, 1
    CMP R0, REG_TEMP1
    JNE check_down
    MOV REG_TEMP1, DIR_RIGHT
    CMP REG_DIRECTION, REG_TEMP1
    JEQ input_done
    MOV REG_DIRECTION, DIR_LEFT
    JMP input_done

check_down:
    MOV R0, KEY_DOWN
    INT 0x25
    MOV REG_TEMP1, 1
    CMP R0, REG_TEMP1
    JNE check_up
    MOV REG_TEMP1, DIR_UP
    CMP REG_DIRECTION, REG_TEMP1
    JEQ input_done
    MOV REG_DIRECTION, DIR_DOWN
    JMP input_done

check_up:
    MOV R0, KEY_UP
    INT 0x25
    MOV REG_TEMP1, 1
    CMP R0, REG_TEMP1
    JNE input_done
    MOV REG_TEMP1, DIR_DOWN
    CMP REG_DIRECTION, REG_TEMP1
    JEQ input_done
    MOV REG_DIRECTION, DIR_UP

input_done:
    RET

update_snake:
    MOV REG_LOOP_IDX, REG_CUR_LEN
    DEC REG_LOOP_IDX

body_shift_loop:
    MOV REG_TEMP1, 0
    CMP REG_LOOP_IDX, REG_TEMP1
    JLE head_update
    MOV REG_TEMP1, REG_LOOP_IDX
    DEC REG_TEMP1
    MOV R0, REG_ADDR_X
    ADD R0, REG_TEMP1
    MOV R1, REG_ADDR_Y
    ADD R1, REG_TEMP1
    LOAD REG_TEMP1, R0
    LOAD REG_TEMP2, R1
    MOV R0, REG_ADDR_X
    ADD R0, REG_LOOP_IDX
    MOV R1, REG_ADDR_Y
    ADD R1, REG_LOOP_IDX
    STORE REG_TEMP1, R0
    STORE REG_TEMP2, R1
    DEC REG_LOOP_IDX
    JMP body_shift_loop

head_update:
    LOAD REG_HEAD_X, REG_ADDR_X
    LOAD REG_HEAD_Y, REG_ADDR_Y
    MOV REG_TEMP1, DIR_RIGHT
    CMP REG_DIRECTION, REG_TEMP1
    JNE check_dir_left
    INC REG_HEAD_X
    JMP store_new_head

check_dir_left:
    MOV REG_TEMP1, DIR_LEFT
    CMP REG_DIRECTION, REG_TEMP1
    JNE check_dir_up
    DEC REG_HEAD_X
    JMP store_new_head

check_dir_up:
    MOV REG_TEMP1, DIR_UP
    CMP REG_DIRECTION, REG_TEMP1
    JNE check_dir_down
    DEC REG_HEAD_Y
    JMP store_new_head

check_dir_down:
    INC REG_HEAD_Y

store_new_head:
    MOV REG_TEMP1, SCREEN_WIDTH
    CMP REG_HEAD_X, REG_TEMP1
    JMPL check_wrap_x_neg
    MOV REG_HEAD_X, 0

check_wrap_x_neg:
    MOV REG_TEMP1, 0
    CMP REG_HEAD_X, REG_TEMP1
    JGE check_wrap_y_pos
    MOV REG_TEMP1, SCREEN_WIDTH
    DEC REG_TEMP1
    MOV REG_HEAD_X, REG_TEMP1

check_wrap_y_pos:
    MOV REG_TEMP1, SCREEN_HEIGHT
    CMP REG_HEAD_Y, REG_TEMP1
    JMPL check_wrap_y_neg
    MOV REG_HEAD_Y, 0

check_wrap_y_neg:
    MOV REG_TEMP1, 0
    CMP REG_HEAD_Y, REG_TEMP1
    JGE done_head_update
    MOV REG_TEMP1, SCREEN_HEIGHT
    DEC REG_TEMP1
    MOV REG_HEAD_Y, REG_TEMP1

done_head_update:
    STORE REG_HEAD_X, REG_ADDR_X
    STORE REG_HEAD_Y, REG_ADDR_Y
    RET

draw_screen:
    MOV R0, BG_COLOR
    INT 0x15
    MOV REG_LOOP_IDX, 0

draw_loop:
    CMP REG_LOOP_IDX, REG_CUR_LEN
    JGE draw_done
    MOV R0, REG_ADDR_X
    ADD R0, REG_LOOP_IDX
    MOV R1, REG_ADDR_Y
    ADD R1, REG_LOOP_IDX
    LOAD R0, R0
    LOAD R1, R1
    MOV R2, SNAKE_COLOR
    INT 0x14
    INC REG_LOOP_IDX
    JMP draw_loop

draw_done:
    RET
