#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifdef _WIN32
#include "Windows.h"
#else
#include <unistd.h>
#endif
#include <time.h>
#include <SDL.h>
#include <stdbool.h>

#define MEMORY_SIZE (128 * 1024)
#define NUM_REGISTERS 16
#define MAX_PROGRAM_SIZE 4096
#define MAX_LINE_LENGTH 256
#define DEFAULT_SCREEN_WIDTH 256
#define DEFAULT_SCREEN_HEIGHT 192
#define MAX_SCREEN_DIM 1024
#define PALETTE_SIZE 16
#define MAX_DEFINES 256 

typedef enum {
    MOV, ADD, SUB, MUL, DIV, INTR, NOP, HLT, NOT, AND, OR, XOR, SHL, SHR, JMP,
    CMP, JNE, JMPH, JMPL, NEG, INC, DEC, XCHG, CLR, PUSH, POP, CALL, RET, ROL,
    ROR, STRMOV, RND, JEQ, MOD, POW, SQRT, ABS, LOOP, LOAD, STORE, TEST,
    LEA, PUSHF, POPF, LOOPE, LOOPNE, SETF, CLRF,
    INVALID_INST
} InstructionType;

typedef struct {
    char name[64];
    char value[64];
} DefineEntry;

#define FLAG_ZERO   0x01
#define FLAG_GREATER 0x02
#define FLAG_LESS   0x04

#define INT_PRINT_REG0      0x01 // Print integer in R0
#define INT_PRINT_STRING    0x02 // Print string from memory address in R0 until null terminator
#define INT_PRINT_NEWLINE   0x03 // Print a newline character
#define INT_SLEEP_MS        0x04 // Sleep for milliseconds specified in R0
#define INT_GET_CHAR        0x05 // Keyboard input, reads a character into R0
#define INT_RESET_CPU       0x06 // Full CPU reset (state, memory, registers, flags, ip)
#define INT_GET_TIME        0x07 // Get current time into R0 (H), R1 (M), R2 (S)
#define INT_CLEAR_SCREEN_OS 0x08 // Clear host OS console screen
#define INT_SPEAKER_ON      0x09 // Turn the virtual speaker on
#define INT_SPEAKER_OFF     0x10 // Turn the virtual speaker off
#define INT_SET_FREQ        0x11 // Set speaker frequency from R0 (Hz)
#define INT_SET_VOLUME      0x12 // Set speaker volume from R0 (0-100)
#define INT_SPEAKER_SLEEP   0x13 // Make speaker silent for R0 milliseconds
#define INT_DRAW_PIXEL      0x14 // Draw pixel at (R0, R1) with palette color index R2
#define INT_CLEAR_GFX_SCREEN 0x15 // Clear graphics screen with palette color index R0
#define INT_SCREEN_ON       0x16 // Enable graphics screen updates
#define INT_SCREEN_OFF      0x17 // Disable graphics screen updates
#define INT_SET_RESOLUTION  0x18 // Change screen resolution to R0 x R1
#define INT_GET_STRING      0x19 // Read string input into memory at R0, max length R1
#define INT_PRINT_HEX_REG0  0x20 // Print R0 as hexadecimal
#define INT_DUMP_REGISTERS  0x21 // Print all register values, IP, SP, Flags
#define INT_DUMP_MEMORY     0x22 // Dump memory from R0 for length R1
#define INT_GET_TICKS       0x23 // Get SDL Ticks (ms since init) into R0

typedef struct {
    int registers[NUM_REGISTERS];
    int memory[MEMORY_SIZE];
    int ip;
    int flags;

    SDL_AudioSpec audioSpec;
    SDL_AudioDeviceID audioDevice;
    double frequency;
    double volume;
    int speaker_on;
    int sleep_duration;
    Uint32 sleep_start_time;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int screen_width;
    int screen_height;
    Uint32* pixels;
    SDL_Color palette[PALETTE_SIZE];
    int screen_on;

    bool halted_by_hlt;

} VirtualCPU;


bool init_cpu(VirtualCPU* cpu);
void cleanup_cpu(VirtualCPU* cpu);
bool init_sdl(VirtualCPU* cpu);
void cleanup_sdl(VirtualCPU* cpu);

InstructionType parseInstruction(const char* instruction);
int loadProgram(const char* filename, char* program[], int max_size);

void execute(VirtualCPU* cpu, char* program[], int program_size);
void interrupt(VirtualCPU* cpu, int interrupt_id);
void updateScreen(VirtualCPU* cpu);

void mov(VirtualCPU* cpu, int reg1, int value);
void add(VirtualCPU* cpu, int reg1, int reg2);
void sub(VirtualCPU* cpu, int reg1, int reg2);
void mul(VirtualCPU* cpu, int reg1, int reg2);
void divi(VirtualCPU* cpu, int reg1, int reg2);
void nop(VirtualCPU* cpu);
void hlt(VirtualCPU* cpu, bool* running_flag);
void not_op(VirtualCPU* cpu, int reg1);
void and_op(VirtualCPU* cpu, int reg1, int reg2);
void or_op(VirtualCPU* cpu, int reg1, int reg2);
void xor_op(VirtualCPU* cpu, int reg1, int reg2);
void shl(VirtualCPU* cpu, int reg1, int count);
void shr(VirtualCPU* cpu, int reg1, int count);
void jmp(VirtualCPU* cpu, int line_number);
void cmp(VirtualCPU* cpu, int reg1, int reg2);
void neg(VirtualCPU* cpu, int reg1);
void inc(VirtualCPU* cpu, int reg1);
void dec(VirtualCPU* cpu, int reg1);
void xchg(VirtualCPU* cpu, int reg1, int reg2);
void clr(VirtualCPU* cpu, int reg1);
void push(VirtualCPU* cpu, int reg1);
void pop(VirtualCPU* cpu, int reg1);
void call(VirtualCPU* cpu, int line_number);
void ret(VirtualCPU* cpu);
void rol(VirtualCPU* cpu, int reg1, int count);
void ror(VirtualCPU* cpu, int reg1, int count);
void rnd(VirtualCPU* cpu, int reg1);
void mod_op(VirtualCPU* cpu, int reg1, int reg2);
void pow_op(VirtualCPU* cpu, int reg1, int reg2);
void sqrt_op(VirtualCPU* cpu, int reg1);
void abs_op(VirtualCPU* cpu, int reg1);
void loop_op(VirtualCPU* cpu, int counter_reg, int target_line);
void load_op(VirtualCPU* cpu, int dest_reg, int addr_src_reg);
void store_op(VirtualCPU* cpu, int val_src_reg, int addr_dest_reg);
void test_op(VirtualCPU* cpu, int reg1, int reg2);
void lea_op(VirtualCPU* cpu, int dest_reg, int base_reg, int offset);
void pushf_op(VirtualCPU* cpu);
void popf_op(VirtualCPU* cpu);
void loope_op(VirtualCPU* cpu, int counter_reg, int target_line);
void loopne_op(VirtualCPU* cpu, int counter_reg, int target_line);
void setf_op(VirtualCPU* cpu, int flag_mask);
void clrf_op(VirtualCPU* cpu, int flag_mask);

void audioCallback(void* userdata, Uint8* stream, int len);

char* strdup_portable(const char* s) {
    if (s == NULL) {
        return NULL;
    }
#ifdef _MSC_VER
    return _strdup(s);
#else
    size_t len = strlen(s) + 1;
    char* new_s = malloc(len);
    if (new_s == NULL) {
        return NULL;
    }
    memcpy(new_s, s, len);
    return new_s;
#endif
}

bool init_cpu(VirtualCPU* cpu) {
    if (!cpu) return false;

    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->memory, 0, sizeof(cpu->memory));
    cpu->ip = 0;
    cpu->flags = 0;
    cpu->registers[15] = MEMORY_SIZE;
    cpu->halted_by_hlt = false;

    cpu->audioDevice = 0;
    cpu->frequency = 440.0;
    cpu->volume = 0.5;
    cpu->speaker_on = 0;
    cpu->sleep_duration = 0;
    cpu->sleep_start_time = 0;

    cpu->window = NULL;
    cpu->renderer = NULL;
    cpu->texture = NULL;
    cpu->screen_width = DEFAULT_SCREEN_WIDTH;
    cpu->screen_height = DEFAULT_SCREEN_HEIGHT;
    cpu->screen_on = 0;

    cpu->pixels = (Uint32*)malloc(cpu->screen_width * cpu->screen_height * sizeof(Uint32));
    if (!cpu->pixels) {
        fprintf(stderr, "Error: Failed to allocate pixel buffer\n");
        return false;
    }
    memset(cpu->pixels, 0, cpu->screen_width * cpu->screen_height * sizeof(Uint32));

    cpu->palette[0] = (SDL_Color){ 0, 0, 0, 255 };
    cpu->palette[1] = (SDL_Color){ 0, 0, 170, 255 };
    cpu->palette[2] = (SDL_Color){ 0, 170, 0, 255 };
    cpu->palette[3] = (SDL_Color){ 0, 170, 170, 255 };
    cpu->palette[4] = (SDL_Color){ 170, 0, 0, 255 };
    cpu->palette[5] = (SDL_Color){ 170, 0, 170, 255 };
    cpu->palette[6] = (SDL_Color){ 170, 85, 0, 255 };
    cpu->palette[7] = (SDL_Color){ 170, 170, 170, 255 };
    cpu->palette[8] = (SDL_Color){ 85, 85, 85, 255 };
    cpu->palette[9] = (SDL_Color){ 85, 85, 255, 255 };
    cpu->palette[10] = (SDL_Color){ 85, 255, 85, 255 };
    cpu->palette[11] = (SDL_Color){ 85, 255, 255, 255 };
    cpu->palette[12] = (SDL_Color){ 255, 85, 85, 255 };
    cpu->palette[13] = (SDL_Color){ 255, 85, 255, 255 };
    cpu->palette[14] = (SDL_Color){ 255, 255, 85, 255 };
    cpu->palette[15] = (SDL_Color){ 255, 255, 255, 255 };

    srand(time(NULL));

    return true;
}

void cleanup_cpu(VirtualCPU* cpu) {
    if (cpu) {
        free(cpu->pixels);
        cpu->pixels = NULL;
    }
}

bool init_sdl(VirtualCPU* cpu) {
    if (!cpu) return false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        return false;
    }

    cpu->window = SDL_CreateWindow("Virtual CPU Screen",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        cpu->screen_width, cpu->screen_height,
        SDL_WINDOW_SHOWN);
    if (!cpu->window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    cpu->renderer = SDL_CreateRenderer(cpu->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!cpu->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(cpu->window);
        SDL_Quit();
        return false;
    }

    cpu->texture = SDL_CreateTexture(cpu->renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        cpu->screen_width, cpu->screen_height);
    if (!cpu->texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error: %s", SDL_GetError());
        SDL_DestroyRenderer(cpu->renderer);
        SDL_DestroyWindow(cpu->window);
        SDL_Quit();
        return false;
    }

    SDL_AudioSpec desiredSpec, obtainedSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 512;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = cpu;

    cpu->audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);
    if (cpu->audioDevice == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenAudioDevice Error: %s", SDL_GetError());
        SDL_DestroyTexture(cpu->texture);
        SDL_DestroyRenderer(cpu->renderer);
        SDL_DestroyWindow(cpu->window);
        SDL_Quit();
        return false;
    }
    cpu->audioSpec = obtainedSpec;

    SDL_PauseAudioDevice(cpu->audioDevice, 0);

    return true;
}

void cleanup_sdl(VirtualCPU* cpu) {
    if (cpu) {
        if (cpu->audioDevice > 0) {
            SDL_CloseAudioDevice(cpu->audioDevice);
        }
        if (cpu->texture) {
            SDL_DestroyTexture(cpu->texture);
        }
        if (cpu->renderer) {
            SDL_DestroyRenderer(cpu->renderer);
        }
        if (cpu->window) {
            SDL_DestroyWindow(cpu->window);
        }
    }
    SDL_Quit();
}

InstructionType parseInstruction(const char* instruction) {
    if (strcmp(instruction, "MOV") == 0) return MOV;
    if (strcmp(instruction, "ADD") == 0) return ADD;
    if (strcmp(instruction, "SUB") == 0) return SUB;
    if (strcmp(instruction, "MUL") == 0) return MUL;
    if (strcmp(instruction, "DIV") == 0) return DIV;
    if (strcmp(instruction, "INT") == 0) return INTR;
    if (strcmp(instruction, "NOP") == 0) return NOP;
    if (strcmp(instruction, "HLT") == 0) return HLT;
    if (strcmp(instruction, "NOT") == 0) return NOT;
    if (strcmp(instruction, "AND") == 0) return AND;
    if (strcmp(instruction, "OR") == 0) return OR;
    if (strcmp(instruction, "XOR") == 0) return XOR;
    if (strcmp(instruction, "SHL") == 0) return SHL;
    if (strcmp(instruction, "SHR") == 0) return SHR;
    if (strcmp(instruction, "JMP") == 0) return JMP;
    if (strcmp(instruction, "CMP") == 0) return CMP;
    if (strcmp(instruction, "JNE") == 0) return JNE;
    if (strcmp(instruction, "JMPH") == 0) return JMPH;
    if (strcmp(instruction, "JMPL") == 0) return JMPL;
    if (strcmp(instruction, "NEG") == 0) return NEG;
    if (strcmp(instruction, "INC") == 0) return INC;
    if (strcmp(instruction, "DEC") == 0) return DEC;
    if (strcmp(instruction, "XCHG") == 0) return XCHG;
    if (strcmp(instruction, "CLR") == 0) return CLR;
    if (strcmp(instruction, "PUSH") == 0) return PUSH;
    if (strcmp(instruction, "POP") == 0) return POP;
    if (strcmp(instruction, "CALL") == 0) return CALL;
    if (strcmp(instruction, "RET") == 0) return RET;
    if (strcmp(instruction, "ROL") == 0) return ROL;
    if (strcmp(instruction, "ROR") == 0) return ROR;
    if (strcmp(instruction, "STRMOV") == 0) return STRMOV;
    if (strcmp(instruction, "RND") == 0) return RND;
    if (strcmp(instruction, "JEQ") == 0) return JEQ;
    if (strcmp(instruction, "MOD") == 0) return MOD;
    if (strcmp(instruction, "POW") == 0) return POW;
    if (strcmp(instruction, "SQRT") == 0) return SQRT;
    if (strcmp(instruction, "ABS") == 0) return ABS;
    if (strcmp(instruction, "LOOP") == 0) return LOOP;
    if (strcmp(instruction, "LOAD") == 0) return LOAD;
    if (strcmp(instruction, "STORE") == 0) return STORE;
    if (strcmp(instruction, "TEST") == 0) return TEST;
    if (strcmp(instruction, "LEA") == 0) return LEA;
    if (strcmp(instruction, "PUSHF") == 0) return PUSHF;
    if (strcmp(instruction, "POPF") == 0) return POPF;
    if (strcmp(instruction, "LOOPE") == 0) return LOOPE;
    if (strcmp(instruction, "LOOPZ") == 0) return LOOPE; // Synonym
    if (strcmp(instruction, "LOOPNE") == 0) return LOOPNE;
    if (strcmp(instruction, "LOOPNZ") == 0) return LOOPNE; // Synonym
    if (strcmp(instruction, "SETF") == 0) return SETF;
    if (strcmp(instruction, "CLRF") == 0) return CLRF;
    return INVALID_INST;
}

int loadProgram(const char* filename, char* program[], int max_size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open program file '%s'\n", filename);
        return -1;
    }

    char buffer[MAX_LINE_LENGTH];
    char* temp_program[MAX_PROGRAM_SIZE];
    int temp_line_count = 0;

    DefineEntry define_map[MAX_DEFINES];
    int define_count = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL && temp_line_count < max_size) {
        buffer[strcspn(buffer, "\n")] = 0;
        char* comment_start = strchr(buffer, ';');
        if (comment_start != NULL) {
            *comment_start = '\0';
        }
        char* start = buffer;
        while (isspace((unsigned char)*start)) start++;
        char* end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        if (strncmp(start, "#define", 7) == 0 && isspace((unsigned char)start[7])) {
            char define_name[64];
            char define_value[64];
            int scan_count = sscanf(start + 7, " %63s %63s", define_name, define_value);

            if (scan_count == 2) {
                if (define_count < MAX_DEFINES) {
                    bool duplicate = false;
                    for (int k = 0; k < define_count; ++k) {
                        if (strcmp(define_map[k].name, define_name) == 0) {
                            fprintf(stderr, "Warning: Duplicate define for '%s' ignored.\n", define_name);
                            duplicate = true;
                            break;
                        }
                    }
                    if (!duplicate) {
                        strcpy(define_map[define_count].name, define_name);
                        strcpy(define_map[define_count].value, define_value);
                        define_count++;
                    }
                }
                else {
                    fprintf(stderr, "Error: Maximum number of defines (%d) exceeded.\n", MAX_DEFINES);
                }
            }
            else {
                fprintf(stderr, "Warning: Invalid #define format: '%s'. Skipping.\n", start);
            }
            continue;
        }

        if (strlen(start) == 0) {
            continue;
        }
        temp_program[temp_line_count] = strdup_portable(start);
        if (!temp_program[temp_line_count]) {
            fprintf(stderr, "Error: Memory allocation failed during program load.\n");
            for (int i = 0; i < temp_line_count; ++i) free(temp_program[i]);
            fclose(file);
            return -2;
        }
        temp_line_count++;
    }
    fclose(file);

    if (temp_line_count >= max_size && fgets(buffer, sizeof(buffer), file) != NULL) {
        fprintf(stderr, "Error: Program exceeds maximum size of %d lines.\n", max_size);
        for (int i = 0; i < temp_line_count; ++i) free(temp_program[i]);
        return -3;
    }

    for (int i = 0; i < temp_line_count; i++) {
        char* original_line = temp_program[i];
        if (!original_line) continue; 

        char processed_line[MAX_LINE_LENGTH * 2] = { 0 }; 
        char token_buffer[MAX_LINE_LENGTH];
        char* current_pos = original_line;
        bool line_changed = false;

        char op_check[10];
        sscanf(original_line, "%9s", op_check);
        bool is_strmov = (strcmp(op_check, "STRMOV") == 0);
        char* quote_start = is_strmov ? strchr(original_line, '"') : NULL;
        char* first_token_end = strpbrk(original_line, " \t,");

        if (first_token_end) {
            strncpy(processed_line, original_line, first_token_end - original_line);
            current_pos = first_token_end;
        }
        else {
            strcat(processed_line, original_line);
            current_pos = original_line + strlen(original_line);
        }

        while (*current_pos != '\0') {
            char* separator_start = current_pos;
            while (*current_pos != '\0' && (isspace((unsigned char)*current_pos) || *current_pos == ',')) {
                current_pos++;
            }
            if (current_pos > separator_start) {
                strncat(processed_line, separator_start, current_pos - separator_start);
            }
            if (*current_pos == '\0') break;

            if (quote_start && current_pos >= quote_start) {
                strcat(processed_line, current_pos);
                current_pos += strlen(current_pos);
                break;
            }

            char* token_start = current_pos;
            int token_len = 0;
            while (*current_pos != '\0' && !isspace((unsigned char)*current_pos) && *current_pos != ',') {
                current_pos++;
                token_len++;
            }
            if (token_len == 0) break;
            if (token_len >= MAX_LINE_LENGTH) {
                fprintf(stderr, "Error: Token too long near '%.*s' on line %d\n", 20, token_start, i + 1);
                token_len = MAX_LINE_LENGTH - 1;
                strncpy(token_buffer, token_start, token_len);
                token_buffer[token_len] = '\0';
                strcat(processed_line, token_buffer);
                continue;
            }
            strncpy(token_buffer, token_start, token_len);
            token_buffer[token_len] = '\0';

            bool replaced = false;
            if (token_buffer[0] != 'R' || !isdigit((unsigned char)token_buffer[1])) {
                if (!isdigit((unsigned char)token_buffer[0]) && token_buffer[0] != '-') {
                    for (int j = 0; j < define_count; j++) {
                        if (strcmp(token_buffer, define_map[j].name) == 0) {
                            strcat(processed_line, define_map[j].value);
                            replaced = true;
                            line_changed = true;
                            break;
                        }
                    }
                }
            }

            if (!replaced) {
                strcat(processed_line, token_buffer);
            }
        }

        if (line_changed) {
            free(temp_program[i]);
            temp_program[i] = strdup_portable(processed_line);
            if (!temp_program[i]) {
                fprintf(stderr, "Error: Memory allocation failed during define substitution.\n");
            }
        }
    }

    typedef struct {
        char label_name[64];
        int line_number;
    } LabelEntry;
    LabelEntry label_map[MAX_PROGRAM_SIZE];
    int label_count = 0;
    int final_program_size = 0;

    for (int i = 0; i < temp_line_count; i++) {
        char* line = temp_program[i];
        char* colon_pos = strchr(line, ':');

        if (colon_pos != NULL) {
            *colon_pos = '\0';
            char* label_start = line;
            while (isspace((unsigned char)*label_start)) label_start++;
            char* label_end = label_start + strlen(label_start) - 1;
            while (label_end > label_start && isspace((unsigned char)*label_end)) label_end--;
            *(label_end + 1) = '\0';

            if (strlen(label_start) > 0 && strlen(label_start) < sizeof(label_map[0].label_name)) {
                bool duplicate = false;
                for (int k = 0; k < label_count; ++k) {
                    if (strcmp(label_map[k].label_name, label_start) == 0) {
                        fprintf(stderr, "Error: Duplicate label '%s' found near line %d\n", label_start, i + 1);
                        duplicate = true;
                        break;
                    }
                }
                if (duplicate) continue;

                strcpy(label_map[label_count].label_name, label_start);
                label_map[label_count].line_number = final_program_size + 1;
                label_count++;

                char* code_after_label = colon_pos + 1;
                while (isspace((unsigned char)*code_after_label)) code_after_label++;
                if (strlen(code_after_label) > 0) {
                    program[final_program_size] = strdup_portable(code_after_label);
                    if (!program[final_program_size]) {
                        fprintf(stderr, "Error: Memory allocation failed for code after label.\n");
                        return -2;
                    }
                    final_program_size++;
                }

            }
            else if (strlen(label_start) > 0) {
                fprintf(stderr, "Warning: Label '%s' near line %d is too long or invalid. Ignored.\n", label_start, i + 1);
            }
            free(temp_program[i]);
            temp_program[i] = NULL;
        }
        else {
            if (final_program_size < max_size) {
                program[final_program_size++] = temp_program[i];
                temp_program[i] = NULL;
            }
            else {
                fprintf(stderr, "Error: Program size exceeded during label processing.\n");
                free(temp_program[i]);
                return -3;
            }
        }
    }

    for (int i = 0; i < final_program_size; i++) {
        char* instruction = program[i];
        char op[10];
        char operand1_str[MAX_LINE_LENGTH];
        char operand2_str[MAX_LINE_LENGTH];

        int scan_count = sscanf(instruction, "%9s %[^,], %s", op, operand1_str, operand2_str);
        if (scan_count < 2) {
            continue;
        }

        InstructionType inst = parseInstruction(op);

        char* label_operand_str = NULL;

        if (inst == JMP || inst == JNE || inst == JMPH || inst == JMPL || inst == CALL || inst == JEQ) {
            if (scan_count >= 2) label_operand_str = operand1_str;
        }
        else if (inst == LOOP || inst == LOOPE || inst == LOOPNE) {
            if (scan_count == 3) label_operand_str = operand2_str;
        }


        if (label_operand_str != NULL && !isdigit((unsigned char)label_operand_str[0]) && label_operand_str[0] != '-') {
            int target_line = -1;
            for (int j = 0; j < label_count; j++) {
                if (strcmp(label_map[j].label_name, label_operand_str) == 0) {
                    target_line = label_map[j].line_number;
                    break;
                }
            }

            if (target_line != -1) {
                char new_instruction[MAX_LINE_LENGTH];  
                if (inst == LOOP || inst == LOOPE || inst == LOOPNE) {
                    snprintf(new_instruction, sizeof(new_instruction), "%s %s, %d", op, operand1_str, target_line);
                }
                else {
                    snprintf(new_instruction, sizeof(new_instruction), "%s %d", op, target_line);
                }

                free(program[i]);
                program[i] = strdup_portable(new_instruction);
                if (!program[i]) {
                    fprintf(stderr, "Error: Memory allocation failed during label resolution.\n");
                    return -2;
                }
            }
            else {
                fprintf(stderr, "Error: Label '%s' not found (used in line %d: '%s')\n", label_operand_str, i + 1, instruction);
            }
        }
        else if (label_operand_str == NULL && (inst == JMP || inst == JNE || inst == JMPH || inst == JMPL || inst == CALL || inst == JEQ || inst == LOOP || inst == LOOPE || inst == LOOPNE)) {
            fprintf(stderr, "Error: Missing or invalid label operand for %s instruction at line %d: '%s'\n", op, i + 1, instruction);
        }
    }

    for (int i = 0; i < temp_line_count; ++i) {
        if (temp_program[i]) free(temp_program[i]);
    }

    return final_program_size;
}


void updateScreen(VirtualCPU* cpu) {
    if (!cpu || !cpu->screen_on || !cpu->texture || !cpu->renderer || !cpu->pixels) {
        return;
    }
    if (SDL_UpdateTexture(cpu->texture, NULL, cpu->pixels, cpu->screen_width * sizeof(Uint32)) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "SDL_UpdateTexture failed: %s", SDL_GetError());
        return;
    }
    if (SDL_RenderCopy(cpu->renderer, cpu->texture, NULL, NULL) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "SDL_RenderCopy failed: %s", SDL_GetError());
        return;
    }
    SDL_RenderPresent(cpu->renderer);
}


void interrupt(VirtualCPU* cpu, int interrupt_id) {
    switch (interrupt_id) {
    case INT_PRINT_REG0:
        printf("%d\n", cpu->registers[0]);
        break;

    case INT_PRINT_STRING:
    {
        int addr = cpu->registers[0];
        if (addr < 0 || addr >= MEMORY_SIZE) {
            fprintf(stderr, "Error (INT 0x02): Invalid memory address in R0: %d\n", addr);
            break;
        }
        int count = 0;
        while (addr < MEMORY_SIZE && cpu->memory[addr] != 0 && count < MEMORY_SIZE) {
            putchar(cpu->memory[addr]);
            addr++;
            count++;
        }
        putchar('\n');
    }
    break;

    case INT_PRINT_NEWLINE:
        putchar('\n');
        break;

    case INT_SLEEP_MS:
    {
        int sleep_ms = cpu->registers[0];
        if (sleep_ms > 0) {
            SDL_Delay((Uint32)sleep_ms);
        }
        else if (sleep_ms < 0) {
            fprintf(stderr, "Warning (INT 0x04): Negative sleep duration (%d ms). Ignoring.\n", sleep_ms);
        }
    }
    break;

    case INT_GET_CHAR:
    {
        int ch = getchar();
        cpu->registers[0] = ch;
    }
    break;

    case INT_RESET_CPU:
    {
        int old_w = cpu->screen_width;
        int old_h = cpu->screen_height;
        Uint32* old_pixels = cpu->pixels;
        SDL_Color old_palette[PALETTE_SIZE];
        memcpy(old_palette, cpu->palette, sizeof(old_palette));
        int old_screen_on = cpu->screen_on;
        int old_speaker_on = cpu->speaker_on;
        double old_freq = cpu->frequency;
        double old_vol = cpu->volume;

        init_cpu(cpu);

        cpu->screen_width = old_w;
        cpu->screen_height = old_h;
        cpu->pixels = old_pixels;
        memcpy(cpu->palette, old_palette, sizeof(cpu->palette));
        cpu->screen_on = old_screen_on;
        cpu->speaker_on = old_speaker_on;
        cpu->frequency = old_freq;
        cpu->volume = old_vol;

        fprintf(stdout, "CPU Reset executed.\n");
    }
    break;

    case INT_GET_TIME:
    {
        time_t timer;
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);
        cpu->registers[0] = tm_info->tm_hour;
        cpu->registers[1] = tm_info->tm_min;
        cpu->registers[2] = tm_info->tm_sec;
    }
    break;

    case INT_CLEAR_SCREEN_OS:
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        break;

    case INT_SPEAKER_ON:
        cpu->speaker_on = 1;
        cpu->sleep_duration = 0;
        break;

    case INT_SPEAKER_OFF:
        cpu->speaker_on = 0;
        cpu->sleep_duration = 0;
        cpu->sleep_start_time = 0;
        break;

    case INT_SET_FREQ:
        SDL_LockAudioDevice(cpu->audioDevice);
        cpu->frequency = (double)cpu->registers[0];
        if (cpu->frequency <= 0) cpu->frequency = 1.0;
        SDL_UnlockAudioDevice(cpu->audioDevice);
        break;

    case INT_SET_VOLUME:
        SDL_LockAudioDevice(cpu->audioDevice);
        cpu->volume = (double)cpu->registers[0] / 100.0;
        if (cpu->volume < 0.0) cpu->volume = 0.0;
        if (cpu->volume > 1.0) cpu->volume = 1.0;
        SDL_UnlockAudioDevice(cpu->audioDevice);
        break;

    case INT_SPEAKER_SLEEP:
        SDL_LockAudioDevice(cpu->audioDevice);
        cpu->sleep_duration = cpu->registers[0];
        cpu->sleep_start_time = SDL_GetTicks();
        SDL_UnlockAudioDevice(cpu->audioDevice);
        break;

    case INT_DRAW_PIXEL:
    {
        int x = cpu->registers[0];
        int y = cpu->registers[1];
        int color_index = cpu->registers[2];

        if (!cpu->pixels) break;

        if (x >= 0 && x < cpu->screen_width && y >= 0 && y < cpu->screen_height &&
            color_index >= 0 && color_index < PALETTE_SIZE)
        {
            SDL_Color palette_color = cpu->palette[color_index];
            Uint32 color = ((Uint32)palette_color.a << 24) |
                ((Uint32)palette_color.r << 16) |
                ((Uint32)palette_color.g << 8) |
                ((Uint32)palette_color.b);
            cpu->pixels[y * cpu->screen_width + x] = color;
        }
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
    }
    break;

    case INT_CLEAR_GFX_SCREEN:
    {
        int color_index = cpu->registers[0];
        if (!cpu->pixels) break;

        if (color_index >= 0 && color_index < PALETTE_SIZE) {
            SDL_Color palette_color = cpu->palette[color_index];
            Uint32 clear_color = ((Uint32)palette_color.a << 24) |
                ((Uint32)palette_color.r << 16) |
                ((Uint32)palette_color.g << 8) |
                ((Uint32)palette_color.b);
            for (int i = 0; i < cpu->screen_width * cpu->screen_height; i++) {
                cpu->pixels[i] = clear_color;
            }
        }
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
    }
    break;

    case INT_SCREEN_ON:
        cpu->screen_on = 1;
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;

    case INT_SCREEN_OFF:
        cpu->screen_on = 0;
        break;

    case INT_SET_RESOLUTION:
    {
        int new_width = cpu->registers[0];
        int new_height = cpu->registers[1];

        if (new_width > 0 && new_height > 0 &&
            new_width <= MAX_SCREEN_DIM && new_height <= MAX_SCREEN_DIM &&
            (new_width != cpu->screen_width || new_height != cpu->screen_height))
        {
            fprintf(stdout, "Attempting to change resolution to %dx%d\n", new_width, new_height);

            Uint32* new_pixels = (Uint32*)realloc(cpu->pixels, new_width * new_height * sizeof(Uint32));
            if (!new_pixels) {
                fprintf(stderr, "Error (INT 0x18): Failed to reallocate pixel buffer for new resolution.\n");
                break;
            }
            cpu->pixels = new_pixels;
            cpu->screen_width = new_width;
            cpu->screen_height = new_height;
            memset(cpu->pixels, 0, cpu->screen_width * cpu->screen_height * sizeof(Uint32));


            SDL_DestroyTexture(cpu->texture);
            cpu->texture = SDL_CreateTexture(cpu->renderer, SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                cpu->screen_width, cpu->screen_height);
            if (!cpu->texture) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error (resolution change): %s", SDL_GetError());
            }

            SDL_SetWindowSize(cpu->window, cpu->screen_width, cpu->screen_height);
        }
        else if (new_width <= 0 || new_height <= 0 || new_width > MAX_SCREEN_DIM || new_height > MAX_SCREEN_DIM) {
            fprintf(stderr, "Warning (INT 0x18): Invalid screen resolution requested: %dx%d. Max is %dx%d.\n",
                new_width, new_height, MAX_SCREEN_DIM, MAX_SCREEN_DIM);
        }
    }
    break;
    case INT_GET_STRING:
    {
        int buffer_addr = cpu->registers[0];
        int max_length = cpu->registers[1];
        char input_buffer[MAX_LINE_LENGTH];

        if (buffer_addr < 0 || buffer_addr >= MEMORY_SIZE) {
            fprintf(stderr, "Error (INT 0x19): Invalid buffer address in R0: %d\n", buffer_addr);
            break;
        }
        if (max_length <= 0) {
            fprintf(stderr, "Warning (INT 0x19): Invalid max length in R1: %d. Reading nothing.\n", max_length);
            if (buffer_addr < MEMORY_SIZE) cpu->memory[buffer_addr] = 0;
            break;
        }
        if (max_length >= MAX_LINE_LENGTH) {
            max_length = MAX_LINE_LENGTH - 1;
        }
        if (buffer_addr + max_length >= MEMORY_SIZE) {
            max_length = MEMORY_SIZE - buffer_addr - 1;
            if (max_length <= 0) {
                fprintf(stderr, "Error (INT 0x19): Buffer address and length exceed memory bounds.\n");
                if (buffer_addr < MEMORY_SIZE) cpu->memory[buffer_addr] = 0;
                break;
            }
        }

        printf("Input: ");
        fflush(stdout);

        if (fgets(input_buffer, max_length + 1, stdin) != NULL) {
            input_buffer[strcspn(input_buffer, "\n")] = 0;
            size_t len = strlen(input_buffer);
            for (size_t i = 0; i < len; i++) {
                if (buffer_addr + i < MEMORY_SIZE) {
                    cpu->memory[buffer_addr + i] = input_buffer[i];
                }
                else {
                    break;
                }
            }
            if (buffer_addr + len < MEMORY_SIZE) {
                cpu->memory[buffer_addr + len] = 0;
            }
            else if (buffer_addr + max_length < MEMORY_SIZE) {
                cpu->memory[buffer_addr + max_length] = 0;
            }
        }
        else {
            fprintf(stderr, "Error reading input for INT 0x19.\n");
            if (buffer_addr < MEMORY_SIZE) cpu->memory[buffer_addr] = 0;
        }
    }
    break;

    case INT_PRINT_HEX_REG0:
        printf("0x%X\n", cpu->registers[0]);
        break;

    case INT_DUMP_REGISTERS:
        printf("--- Register Dump ---\n");
        for (int i = 0; i < NUM_REGISTERS; i++) {
            printf(" R%d: %d (0x%X)\n", i, cpu->registers[i], cpu->registers[i]);
        }
        printf(" IP: %d\n", cpu->ip);
        printf(" Flags: 0x%X (Z:%d G:%d L:%d)\n", cpu->flags,
            (cpu->flags & FLAG_ZERO) ? 1 : 0,
            (cpu->flags & FLAG_GREATER) ? 1 : 0,
            (cpu->flags & FLAG_LESS) ? 1 : 0);
        printf(" SP (R15): %d (0x%X)\n", cpu->registers[15], cpu->registers[15]);
        printf("---------------------\n");
        break;

    case INT_DUMP_MEMORY:
    {
        int addr = cpu->registers[0];
        int length = cpu->registers[1];
        if (addr < 0) addr = 0;
        if (length <= 0) length = 16;
        if (addr + length > MEMORY_SIZE) length = MEMORY_SIZE - addr;

        printf("--- Memory Dump Addr: 0x%X, Len: %d ---\n", addr, length);
        for (int i = 0; i < length; i++) {
            if (i % 16 == 0) {
                printf("%04X: ", addr + i);
            }
            printf("%02X ", cpu->memory[addr + i] & 0xFF);
            if ((i + 1) % 16 == 0 || i == length - 1) {
                printf("\n");
            }
        }
        printf("---------------------------------------\n");
    }
    break;

    case INT_GET_TICKS:
        cpu->registers[0] = SDL_GetTicks();
        break;

    default:
        fprintf(stderr, "Error: Unknown interrupt code 0x%X at line %d\n", interrupt_id, cpu->ip + 1);
        break;
    }
}

bool isValidReg(int reg) {
    return reg >= 0 && reg < NUM_REGISTERS;
}
bool isValidMem(int addr) {
    return addr >= 0 && addr < MEMORY_SIZE;
}

void execute(VirtualCPU* cpu, char* program[], int program_size) {
    bool running = true;
    while (running && cpu->ip < program_size) {
        char* current_instruction_line = program[cpu->ip];
        char op_str[10];
        int operands[3] = { 0, 0, 0 };
        char str_operand[MAX_LINE_LENGTH] = { 0 };

        int parsed_items = sscanf(current_instruction_line, "%9s", op_str);
        if (parsed_items < 1) {
            fprintf(stderr, "Warning: Empty or invalid instruction format at line %d: '%s'. Skipping.\n", cpu->ip + 1, current_instruction_line);
            cpu->ip++;
            continue;
        }

        InstructionType inst = parseInstruction(op_str);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
        }
        if (!running) break;


        int current_ip = cpu->ip;

        switch (inst) {
        case MOV: {
            char operand1_str[64], operand2_str[MAX_LINE_LENGTH];
            int dest_reg = -1;

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error MOV: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    break;
                }

                int src_reg = -1;
                int immediate_val = 0;

                if (operand2_str[0] == 'R' && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error MOV: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                    }
                    else {
                        cpu->registers[dest_reg] = cpu->registers[src_reg];
                    }
                }
                else if (sscanf(operand2_str, "%d", &immediate_val) == 1) {
                    mov(cpu, dest_reg, immediate_val);
                }
                else {
                    fprintf(stderr, "Error: Invalid MOV second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                }
            }
            else {
                fprintf(stderr, "Error: Invalid MOV format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
            }
            break;
        }
        case ADD:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                add(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid ADD format at line %d\n", cpu->ip + 1); }
            break;
        case SUB:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                sub(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid SUB format at line %d\n", cpu->ip + 1); }
            break;
        case MUL:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                mul(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid MUL format at line %d\n", cpu->ip + 1); }
            break;
        case DIV:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                divi(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid DIV format at line %d\n", cpu->ip + 1); }
            break;
        case MOD:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                mod_op(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid MOD format at line %d\n", cpu->ip + 1); }
            break;
        case POW:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                pow_op(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid POW format at line %d\n", cpu->ip + 1); }
            break;
        case SQRT:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                sqrt_op(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid SQRT format at line %d\n", cpu->ip + 1); }
            break;
        case ABS:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                abs_op(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid ABS format at line %d\n", cpu->ip + 1); }
            break;
        case LOOP:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                loop_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid LOOP format at line %d (Expected: LOOP Rx, <line_number>)\n", cpu->ip + 1);
            }
            break;
        case INTR:
            if (sscanf(current_instruction_line, "%*s %x", &operands[0]) == 1) {
                interrupt(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid INT format at line %d\n", cpu->ip + 1); }
            break;
        case NOP:
            nop(cpu);
            break;
        case HLT:
            hlt(cpu, &running);
            break;
        case NOT:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                not_op(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid NOT format at line %d\n", cpu->ip + 1); }
            break;
        case AND:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                and_op(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid AND format at line %d\n", cpu->ip + 1); }
            break;
        case OR:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                or_op(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid OR format at line %d\n", cpu->ip + 1); }
            break;
        case XOR:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                xor_op(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid XOR format at line %d\n", cpu->ip + 1); }
            break;
        case SHL:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                shl(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid SHL format at line %d\n", cpu->ip + 1); }
            break;
        case SHR:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                shr(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid SHR format at line %d\n", cpu->ip + 1); }
            break;
        case ROL:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                rol(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid ROL format at line %d\n", cpu->ip + 1); }
            break;
        case ROR:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                ror(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid ROR format at line %d\n", cpu->ip + 1); }
            break;
        case JMP:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                jmp(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid JMP format at line %d\n", cpu->ip + 1); }
            break;
        case CMP:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                cmp(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid CMP format at line %d\n", cpu->ip + 1); }
            break;
        case JNE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_ZERO) == 0) {
                    jmp(cpu, operands[0]);
                }
            }
            else { fprintf(stderr, "Error: Invalid JNE format at line %d\n", cpu->ip + 1); }
            break;
        case JEQ:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_ZERO) != 0) {
                    jmp(cpu, operands[0]);
                }
            }
            else { fprintf(stderr, "Error: Invalid JEQ format at line %d\n", cpu->ip + 1); }
            break;
        case JMPH:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_GREATER) != 0) {
                    jmp(cpu, operands[0]);
                }
            }
            else { fprintf(stderr, "Error: Invalid JMPH format at line %d\n", cpu->ip + 1); }
            break;
        case JMPL:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_LESS) != 0) {
                    jmp(cpu, operands[0]);
                }
            }
            else { fprintf(stderr, "Error: Invalid JMPL format at line %d\n", cpu->ip + 1); }
            break;
        case NEG:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                neg(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid NEG format at line %d\n", cpu->ip + 1); }
            break;
        case INC:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                inc(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid INC format at line %d\n", cpu->ip + 1); }
            break;
        case DEC:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                dec(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid DEC format at line %d\n", cpu->ip + 1); }
            break;
        case XCHG:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                xchg(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid XCHG format at line %d\n", cpu->ip + 1); }
            break;
        case CLR:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                clr(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid CLR format at line %d\n", cpu->ip + 1); }
            break;
        case PUSH:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                push(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid PUSH format at line %d\n", cpu->ip + 1); }
            break;
        case POP:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                pop(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid POP format at line %d\n", cpu->ip + 1); }
            break;
        case CALL:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                call(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid CALL format at line %d\n", cpu->ip + 1); }
            break;
        case RET:
            ret(cpu);
            break;
        case RND:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                rnd(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid RND format at line %d\n", cpu->ip + 1); }
            break;
        case STRMOV: {
            int addr;
            char str[256];
            if (sscanf(current_instruction_line, "STRMOV %d, \"%[^\"]\"", &addr, str) == 2) {
                if (addr >= 0 && addr < MEMORY_SIZE) {
                    for (int i = 0; str[i] != '\0'; i++) {
                        cpu->memory[addr + i] = str[i];
                    }
                    cpu->memory[addr + strlen(str)] = 0;
                }
                else {
                    fprintf(stderr, "Error: Invalid STRMOV memory address %d at line %d\n", addr, cpu->ip + 1);
                }
            }
            else {
                fprintf(stderr, "Error: Invalid STRMOV format at line %d\n", cpu->ip + 1);
            }
            break;
        }
        case LOAD:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                load_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid LOAD format at line %d (Expected: LOAD Rdest, Raddr_src)\n", cpu->ip + 1);
            }
            break;

        case STORE:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                store_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid STORE format at line %d (Expected: STORE Rval_src, Raddr_dest)\n", cpu->ip + 1);
            }
            break;
        case TEST:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                test_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid TEST format at line %d (Expected: TEST R1, R2)\n", cpu->ip + 1);
            }
            break;
        case LEA: {
            int dest_reg = -1, base_reg = -1, offset = 0;
            char base_reg_str[10];
            if (sscanf(current_instruction_line, "%*s R%d, %[^,], %d", &dest_reg, base_reg_str, &offset) == 3) {
                if (sscanf(base_reg_str, "R%d", &base_reg) == 1) {
                    lea_op(cpu, dest_reg, base_reg, offset);
                }
                else {
                    fprintf(stderr, "Error: Invalid LEA base register format '%s' at line %d\n", base_reg_str, cpu->ip + 1);
                }
            }
            else {
                fprintf(stderr, "Error: Invalid LEA format at line %d (Expected: LEA Rdest, Rbase, offset)\n", cpu->ip + 1);
            }
            break;
        }

        case PUSHF:
            pushf_op(cpu);
            break;

        case POPF:
            popf_op(cpu);
            break;
        case LOOPE:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                loope_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid LOOPE/LOOPZ format at line %d (Expected: OP Rx, <line_number>)\n", cpu->ip + 1);
            }
            break;

        case LOOPNE:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                loopne_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid LOOPNE/LOOPNZ format at line %d (Expected: OP Rx, <line_number>)\n", cpu->ip + 1);
            }
            break;
        case SETF:
            if (sscanf(current_instruction_line, "%*s %x", &operands[0]) == 1) {
                setf_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid SETF format at line %d (Expected: SETF <hex_mask>)\n", cpu->ip + 1);
            }
            break;

        case CLRF:
            if (sscanf(current_instruction_line, "%*s %x", &operands[0]) == 1) {
                clrf_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid CLRF format at line %d (Expected: CLRF <hex_mask> or CLRF 0)\n", cpu->ip + 1);
            }
            break;

        case INVALID_INST:
        default:
            fprintf(stderr, "Error: Unknown or invalid instruction '%s' at line %d\n", op_str, cpu->ip + 1);
            running = false;
            break;
        }

        if (running && cpu->ip == current_ip) {
            cpu->ip++;
        }
    }

    if (cpu->halted_by_hlt) {
        printf("Execution halted by HLT instruction.\n");
    }
    else if (cpu->ip >= program_size) {
        printf("Program finished by running past the last instruction.\n");
    }
    else if (!running) {
        printf("Execution halted (SDL Quit or Error).\n");
    }
    else {
        printf("Execution stopped unexpectedly.\n");
    }
}


void mov(VirtualCPU* cpu, int reg1, int value) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error MOV: Invalid register R%d\n", reg1); return; }
    cpu->registers[reg1] = value;
}

void add(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error ADD: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] += cpu->registers[reg2];
}

void sub(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error SUB: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] -= cpu->registers[reg2];
}

void mul(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error MUL: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] *= cpu->registers[reg2];
}

void divi(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error DIV: Invalid register R%d or R%d\n", reg1, reg2); return; }
    if (cpu->registers[reg2] != 0) {
        cpu->registers[reg1] /= cpu->registers[reg2];
    }
    else {
        fprintf(stderr, "Error: Division by zero (DIV R%d, R%d) at line %d.\n", reg1, reg2, cpu->ip + 1);
    }
}

void mod_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error MOD: Invalid register R%d or R%d\n", reg1, reg2); return; }
    if (cpu->registers[reg2] != 0) {
        cpu->registers[reg1] %= cpu->registers[reg2];
    }
    else {
        fprintf(stderr, "Error: Modulo by zero (MOD R%d, R%d) at line %d.\n", reg1, reg2, cpu->ip + 1);
    }
}

void pow_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error POW: Invalid register R%d or R%d\n", reg1, reg2); return; }
    long long base = cpu->registers[reg1];
    int exponent = cpu->registers[reg2];
    long long result = 1;

    if (exponent < 0) {
        fprintf(stderr, "Warning (POW): Negative exponent %d. Result set to 0.\n", exponent);
        result = 0;
    }
    else if (exponent == 0) {
        result = 1;
    }
    else {
        for (int i = 0; i < exponent; i++) {
            if (llabs(base) > 0 && llabs(result) > INT_MAX / llabs(base)) {
                fprintf(stderr, "Warning (POW): Potential integer overflow. Result truncated.\n");
            }
            result *= base;
        }
    }
    if (result > INT_MAX) cpu->registers[reg1] = INT_MAX;
    else if (result < INT_MIN) cpu->registers[reg1] = INT_MIN;
    else cpu->registers[reg1] = (int)result;
}

void sqrt_op(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error SQRT: Invalid register R%d\n", reg1); return; }
    if (cpu->registers[reg1] >= 0) {
        cpu->registers[reg1] = (int)sqrt((double)cpu->registers[reg1]);
    }
    else {
        fprintf(stderr, "Warning (SQRT): Negative input %d. Result set to 0.\n", cpu->registers[reg1]);
        cpu->registers[reg1] = 0;
    }
}

void abs_op(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error ABS: Invalid register R%d\n", reg1); return; }
    if (cpu->registers[reg1] < 0) {
        if (cpu->registers[reg1] == INT_MIN) {
            fprintf(stderr, "Warning (ABS): Absolute value of INT_MIN requested. Result remains INT_MIN.\n");
        }
        else {
            cpu->registers[reg1] = -cpu->registers[reg1];
        }
    }
}

void loop_op(VirtualCPU* cpu, int counter_reg, int target_line) {
    if (!isValidReg(counter_reg)) {
        fprintf(stderr, "Error LOOP: Invalid counter register R%d at line %d\n", counter_reg, cpu->ip + 1);
        return;
    }

    cpu->registers[counter_reg]--;

    if (cpu->registers[counter_reg] != 0) {
        jmp(cpu, target_line);
    }
}

void loope_op(VirtualCPU* cpu, int counter_reg, int target_line) {
    if (!isValidReg(counter_reg)) {
        fprintf(stderr, "Error LOOPE: Invalid counter register R%d at line %d\n", counter_reg, cpu->ip + 1);
        return;
    }

    cpu->registers[counter_reg]--;

    if (cpu->registers[counter_reg] != 0 && (cpu->flags & FLAG_ZERO) != 0) {
        jmp(cpu, target_line);
    }
    else {
    }
}

void loopne_op(VirtualCPU* cpu, int counter_reg, int target_line) {
    if (!isValidReg(counter_reg)) {
        fprintf(stderr, "Error LOOPNE: Invalid counter register R%d at line %d\n", counter_reg, cpu->ip + 1);
        return;
    }

    cpu->registers[counter_reg]--;

    if (cpu->registers[counter_reg] != 0 && (cpu->flags & FLAG_ZERO) == 0) {
        jmp(cpu, target_line);
    }
    else {
    }
}

void nop(VirtualCPU* cpu) {
    (void)cpu;
}

void hlt(VirtualCPU* cpu, bool* running_flag) {
    (void)cpu;
    printf("HLT encountered at line %d.\n", cpu->ip + 1);
    cpu->halted_by_hlt = true;
    *running_flag = false;
}

void not_op(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error NOT: Invalid register R%d\n", reg1); return; }
    cpu->registers[reg1] = ~cpu->registers[reg1];
}

void and_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error AND: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] &= cpu->registers[reg2];
}

void or_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error OR: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] |= cpu->registers[reg2];
}

void xor_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error XOR: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] ^= cpu->registers[reg2];
}

void shl(VirtualCPU* cpu, int reg1, int count) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error SHL: Invalid register R%d\n", reg1); return; }
    if (count < 0) { fprintf(stderr, "Warning SHL: Negative shift count %d. Ignored.\n", count); return; }
    cpu->registers[reg1] <<= count;
}

void shr(VirtualCPU* cpu, int reg1, int count) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error SHR: Invalid register R%d\n", reg1); return; }
    if (count < 0) { fprintf(stderr, "Warning SHR: Negative shift count %d. Ignored.\n", count); return; }
    cpu->registers[reg1] >>= count;
}

void jmp(VirtualCPU* cpu, int line_number) {
    int target_ip = line_number - 1;
    if (target_ip >= 0) {
        cpu->ip = target_ip;
    }
    else {
        fprintf(stderr, "Error: Invalid jump target line %d (-> index %d) at line %d.\n", line_number, target_ip, cpu->ip + 1);
    }
}

void cmp(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error CMP: Invalid register R%d or R%d\n", reg1, reg2); return; }
    int val1 = cpu->registers[reg1];
    int val2 = cpu->registers[reg2];
    int result = val1 - val2;

    cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS);

    if (result == 0) {
        cpu->flags |= FLAG_ZERO;
    }
    else if (val1 > val2) {
        cpu->flags |= FLAG_GREATER;
    }
    else {
        cpu->flags |= FLAG_LESS;
    }
}

void neg(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error NEG: Invalid register R%d\n", reg1); return; }
    if (cpu->registers[reg1] == INT_MIN) {
        fprintf(stderr, "Warning (NEG): Negation of INT_MIN requested. Result remains INT_MIN.\n");
    }
    else {
        cpu->registers[reg1] = -cpu->registers[reg1];
    }
}

void inc(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error INC: Invalid register R%d\n", reg1); return; }
    if (cpu->registers[reg1] == INT_MAX) {
        fprintf(stderr, "Warning (INC): Integer overflow on increment. Result wrapped to INT_MIN.\n");
        cpu->registers[reg1] = INT_MIN;
    }
    else {
        cpu->registers[reg1]++;
    }
}

void dec(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error DEC: Invalid register R%d\n", reg1); return; }
    if (cpu->registers[reg1] == INT_MIN) {
        fprintf(stderr, "Warning (DEC): Integer underflow on decrement. Result wrapped to INT_MAX.\n");
        cpu->registers[reg1] = INT_MAX;
    }
    else {
        cpu->registers[reg1]--;
    }
}

void xchg(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error XCHG: Invalid register R%d or R%d\n", reg1, reg2); return; }
    int temp = cpu->registers[reg1];
    cpu->registers[reg1] = cpu->registers[reg2];
    cpu->registers[reg2] = temp;
}

void clr(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error CLR: Invalid register R%d\n", reg1); return; }
    cpu->registers[reg1] = 0;
}

void push(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error PUSH: Invalid register R%d\n", reg1); return; }
    int sp = cpu->registers[15];
    sp--;
    if (sp < 0) {
        fprintf(stderr, "Error: Stack Overflow during PUSH at line %d\n", cpu->ip + 1);
        cpu->registers[15] = 0;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during PUSH.\n", sp);
        return;
    }

    cpu->memory[sp] = cpu->registers[reg1];
    cpu->registers[15] = sp;
}

void pop(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error POP: Invalid register R%d\n", reg1); return; }
    int sp = cpu->registers[15];
    if (sp >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Stack Underflow during POP at line %d (SP=%d)\n", cpu->ip + 1, sp);
        cpu->registers[15] = MEMORY_SIZE;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during POP.\n", sp);
        return;
    }

    cpu->registers[reg1] = cpu->memory[sp];
    cpu->registers[15] = sp + 1;
}

void call(VirtualCPU* cpu, int line_number) {
    int target_ip = line_number - 1;
    if (target_ip < 0) {
        fprintf(stderr, "Error: Invalid CALL target line %d (-> index %d) at line %d.\n", line_number, target_ip, cpu->ip + 1);
        return;
    }

    int sp = cpu->registers[15];
    sp--;
    if (sp < 0) {
        fprintf(stderr, "Error: Stack Overflow during CALL at line %d\n", cpu->ip + 1);
        cpu->registers[15] = 0;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during CALL.\n", sp);
        return;
    }

    cpu->memory[sp] = cpu->ip + 1;
    cpu->registers[15] = sp;

    cpu->ip = target_ip;
}

void ret(VirtualCPU* cpu) {
    int sp = cpu->registers[15];
    if (sp >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Stack Underflow during RET at line %d (SP=%d)\n", cpu->ip + 1, sp);
        cpu->registers[15] = MEMORY_SIZE;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during RET.\n", sp);
        return;
    }

    int return_ip = cpu->memory[sp];
    cpu->registers[15] = sp + 1;

    if (return_ip >= 0) {
        cpu->ip = return_ip;
    }
    else {
        fprintf(stderr, "Error: Invalid return address %d popped from stack during RET at line %d.\n", return_ip, cpu->ip + 1);
        cpu->ip = MEMORY_SIZE;
    }
}

void rol(VirtualCPU* cpu, int reg1, int count) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error ROL: Invalid register R%d\n", reg1); return; }
    if (count < 0) { fprintf(stderr, "Warning ROL: Negative rotate count %d. Ignored.\n", count); return; }

    unsigned int val = (unsigned int)cpu->registers[reg1];
    int bit_size = sizeof(int) * 8;
    count %= bit_size;

    if (count > 0) {
        cpu->registers[reg1] = (int)((val << count) | (val >> (bit_size - count)));
    }
}

void ror(VirtualCPU* cpu, int reg1, int count) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error ROR: Invalid register R%d\n", reg1); return; }
    if (count < 0) { fprintf(stderr, "Warning ROR: Negative rotate count %d. Ignored.\n", count); return; }

    unsigned int val = (unsigned int)cpu->registers[reg1];
    int bit_size = sizeof(int) * 8;
    count %= bit_size;

    if (count > 0) {
        cpu->registers[reg1] = (int)((val >> count) | (val << (bit_size - count)));
    }
}

void rnd(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error RND: Invalid register R%d\n", reg1); return; }
    cpu->registers[reg1] = rand();
}

void load_op(VirtualCPU* cpu, int dest_reg, int addr_src_reg) {
    if (!isValidReg(dest_reg)) {
        fprintf(stderr, "Error LOAD: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
        return;
    }
    if (!isValidReg(addr_src_reg)) {
        fprintf(stderr, "Error LOAD: Invalid address source register R%d at line %d\n", addr_src_reg, cpu->ip + 1);
        return;
    }

    int address = cpu->registers[addr_src_reg];

    if (!isValidMem(address)) {
        fprintf(stderr, "Error LOAD: Invalid memory address %d (0x%X) in R%d at line %d\n", address, address, addr_src_reg, cpu->ip + 1);
        return;
    }

    cpu->registers[dest_reg] = cpu->memory[address];
}

void store_op(VirtualCPU* cpu, int val_src_reg, int addr_dest_reg) {
    if (!isValidReg(val_src_reg)) {
        fprintf(stderr, "Error STORE: Invalid value source register R%d at line %d\n", val_src_reg, cpu->ip + 1);
        return;
    }
    if (!isValidReg(addr_dest_reg)) {
        fprintf(stderr, "Error STORE: Invalid address destination register R%d at line %d\n", addr_dest_reg, cpu->ip + 1);
        return;
    }

    int address = cpu->registers[addr_dest_reg];

    if (!isValidMem(address)) {
        fprintf(stderr, "Error STORE: Invalid memory address %d (0x%X) in R%d at line %d\n", address, address, addr_dest_reg, cpu->ip + 1);
        return;
    }

    cpu->memory[address] = cpu->registers[val_src_reg];
}

void test_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error TEST: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (!isValidReg(reg2)) {
        fprintf(stderr, "Error TEST: Invalid register R%d at line %d\n", reg2, cpu->ip + 1);
        return;
    }

    int val1 = cpu->registers[reg1];
    int val2 = cpu->registers[reg2];

    int result = val1 & val2;

    cpu->flags &= ~FLAG_ZERO;
    if (result == 0) {
        cpu->flags |= FLAG_ZERO;
    }
}

void lea_op(VirtualCPU* cpu, int dest_reg, int base_reg, int offset) {
    if (!isValidReg(dest_reg)) {
        fprintf(stderr, "Error LEA: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
        return;
    }
    if (!isValidReg(base_reg)) {
        fprintf(stderr, "Error LEA: Invalid base register R%d at line %d\n", base_reg, cpu->ip + 1);
        return;
    }

    long long calculated_address = (long long)cpu->registers[base_reg] + offset;

    if (calculated_address > INT_MAX || calculated_address < INT_MIN) {
        fprintf(stderr, "Warning LEA: Calculated address %lld overflows standard integer at line %d. Clamping.\n", calculated_address, cpu->ip + 1);
        if (calculated_address > INT_MAX) calculated_address = INT_MAX;
        if (calculated_address < INT_MIN) calculated_address = INT_MIN;
    }

    cpu->registers[dest_reg] = (int)calculated_address;
}

void pushf_op(VirtualCPU* cpu) {
    int sp = cpu->registers[15];
    sp--;

    if (sp < 0) {
        fprintf(stderr, "Error: Stack Overflow during PUSHF at line %d\n", cpu->ip + 1);
        cpu->registers[15] = 0;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during PUSHF at line %d.\n", sp, cpu->ip + 1);
        return;
    }

    cpu->memory[sp] = cpu->flags;
    cpu->registers[15] = sp;
}

void popf_op(VirtualCPU* cpu) {
    int sp = cpu->registers[15];

    if (sp >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Stack Underflow during POPF at line %d (SP=%d)\n", cpu->ip + 1, sp);
        cpu->registers[15] = MEMORY_SIZE;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during POPF at line %d.\n", sp, cpu->ip + 1);
        return;
    }

    cpu->flags = cpu->memory[sp];
    cpu->registers[15] = sp + 1;
}

void setf_op(VirtualCPU* cpu, int flag_mask) {
    cpu->flags |= flag_mask;
}

void clrf_op(VirtualCPU* cpu, int flag_mask) {
    if (flag_mask == 0) {
        cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS);
    }
    else {
        cpu->flags &= ~flag_mask;
    }
}

void audioCallback(void* userdata, Uint8* stream, int len) {
    VirtualCPU* cpu = (VirtualCPU*)userdata;
    Sint16* audio_stream = (Sint16*)stream;
    int samples_to_generate = len / sizeof(Sint16);

    static double phase = 0.0;
    double phase_increment = cpu->frequency / (double)cpu->audioSpec.freq;

    for (int i = 0; i < samples_to_generate; i++) {
        Sint16 sample_value = 0;

        if (cpu->speaker_on) {
            bool is_sleeping = false;
            if (cpu->sleep_duration > 0) {
                Uint32 current_time = SDL_GetTicks();
                if (current_time - cpu->sleep_start_time < (Uint32)cpu->sleep_duration) {
                    is_sleeping = true;
                }
                else {
                    cpu->sleep_duration = 0;
                    cpu->sleep_start_time = 0;
                    is_sleeping = false;
                }
            }
            if (!is_sleeping) {
                double wave_value = sin(2.0 * M_PI * phase);
                sample_value = (wave_value > 0) ? (Sint16)(32767 * cpu->volume) : (Sint16)(-32767 * cpu->volume);
            }
        }

        audio_stream[i] = sample_value;
        phase += phase_increment;
        if (phase >= 1.0) phase -= 1.0;
    }
}


int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    char* program[MAX_PROGRAM_SIZE];
    const char* filename = "program.asm";
    int program_size = 0;
    VirtualCPU cpu;
    bool sdl_initialized = false;
    bool cpu_initialized = false;

    printf("Initializing Virtual CPU...\n");
    if (!init_cpu(&cpu)) {
        fprintf(stderr, "CPU Initialization failed.\n");
        return 1;
    }
    cpu_initialized = true;
    printf("CPU Initialized.\n");


    printf("Initializing SDL...\n");
    if (!init_sdl(&cpu)) {
        fprintf(stderr, "SDL Initialization failed.\n");
        if (cpu_initialized) cleanup_cpu(&cpu);
        return 1;
    }
    sdl_initialized = true;
    printf("SDL Initialized.\n");


    printf("Loading program '%s'...\n", filename);
    program_size = loadProgram(filename, program, MAX_PROGRAM_SIZE);

    if (program_size < 0) {
        fprintf(stderr, "Failed to load program (Error code: %d).\n", program_size);
        if (sdl_initialized) cleanup_sdl(&cpu);
        if (cpu_initialized) cleanup_cpu(&cpu);
        return 1;
    }
    printf("Program loaded successfully (%d lines).\n", program_size);


    printf("Starting execution...\n");
    execute(&cpu, program, program_size);

    printf("Cleaning up...\n");
    for (int i = 0; i < program_size; i++) {
        free(program[i]);
        program[i] = NULL;
    }
    printf("Program memory freed.\n");

    if (sdl_initialized) {
        cleanup_sdl(&cpu);
        printf("SDL resources cleaned up.\n");
    }

    if (cpu_initialized) {
        cleanup_cpu(&cpu);
        printf("CPU resources cleaned up.\n");
    }

    printf("Exiting.\n");
    return 0;
}
