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

#define CPU_VERSION 1
#define MEMORY_SIZE (1024 * 1024)
#define NUM_REGISTERS 16
#define MAX_PROGRAM_SIZE 65536
#define MAX_LINE_LENGTH 512
#define DEFAULT_SCREEN_WIDTH 256
#define DEFAULT_SCREEN_HEIGHT 192
#define MAX_SCREEN_DIM 1024
#define PALETTE_SIZE 16
#define MAX_DEFINES 4096

typedef enum {
    MOV, ADD, SUB, MUL, DIV, INTR, NOP, HLT, NOT, AND, OR, XOR, SHL, SHR, JMP,
    CMP, JNE, JMPH, JMPL, NEG, INC, DEC, XCHG, CLR, PUSH, POP, CALL, RET, ROL,
    ROR, STRMOV, RND, JEQ, MOD, POW, SQRT, ABS, LOOP, LOAD, STORE, TEST,
    LEA, PUSHF, POPF, LOOPE, LOOPNE, SETF, CLRF, BT, BSET, BCLR, BTOG,
    STRCMP, STRLEN, STRCPY, MEMCPY, MEMSET, CPUID,
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
#define INT_GET_PIXEL       0x24 // Get pixel color index at (R0, R1) into R0 (-1 if invalid)
#define INT_GET_KEY_STATE   0x25 // Get state of key (SDL_Scancode in R0). R0=1 if pressed, 0 if not.
#define INT_WAIT_FOR_KEY    0x26 // Waits for a key press, returns SDL_Scancode in R0.
#define INT_GET_MOUSE_STATE 0x27 // Get mouse state: R0=X, R1=Y, R2=ButtonMask (1=L, 2=M, 4=R)
#define INT_GET_MEMORY_SIZE 0x28 // Get configured MEMORY_SIZE into R0
#define INT_FILE_OPEN       0x29 // Open file (R0=name_addr, R1=mode_addr). Returns handle in R0 (-1 error).
#define INT_FILE_CLOSE      0x2A // Close file (R0=handle). Returns 0/-1 in R0.
#define INT_FILE_READ       0x2B // Read file (R0=handle, R1=mem_addr, R2=bytes). Returns bytes read in R0 (-1 error).
#define INT_FILE_WRITE      0x2C // Write file (R0=handle, R1=mem_addr, R2=bytes). Returns bytes written in R0 (-1 error).
#define INT_FILE_SEEK       0x2D // Seek file (R0=handle, R1=offset, R2=origin[0=SET,1=CUR,2=END]). Returns 0/-1 in R0.
#define INT_FILE_TELL       0x2E // Tell file pos (R0=handle). Returns position in R0 (-1 error).
#define INT_SET_CONSOLE_COLOR 0x2F // Set console text color (R0=FG, R1=BG [Win32 codes])
#define INT_RESET_CONSOLE_COLOR 0x30 // Reset console color to default
#define INT_GOTOXY          0x31 // Move console cursor (R0=Col, R1=Row [0-based])
#define INT_GETXY           0x32 // Get console cursor pos (R0=Col, R1=Row) [Win32 only reliable]
#define INT_GET_CONSOLE_SIZE 0x33 // Get console dimensions (R0=Width, R1=Height)
#define INT_SET_CONSOLE_TITLE 0x34 // Set console window title (R0=title_addr)
#define INT_SYSTEM_SHUTDOWN 0x35 // Requests the emulator session to end
#define INT_GET_DATETIME    0x36 // Get Date/Time (R0=Y, R1=M, R2=D, R3=h, R4=m, R5=s)
#define INT_DISK_READ       0x37 // Read sectors (R0=Sector#, R1=MemAddr, R2=NumSectors) -> R0=Status
#define INT_DISK_WRITE      0x38 // Write sectors (R0=Sector#, R1=MemAddr, R2=NumSectors) -> R0=Status
#define INT_DISK_INFO       0x39 // Get disk info -> R0=TotalSectors, R1=SectorSize
#define INT_BREAKPOINT      0x3A // Trigger host debugger breakpoint (if attached and set)

#define MAX_OPEN_FILES 16

#define DISK_IMAGE_FILENAME "disk.img"
#define DISK_IMAGE_SIZE_BYTES (16 * 1024 * 1024) // 16 MB
#define DISK_SECTOR_SIZE 512

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

    bool shutdown_requested;

    FILE* open_files[MAX_OPEN_FILES];
    bool file_slot_used[MAX_OPEN_FILES];

    FILE* disk_image_fp;
    long long disk_image_size;
    int disk_sector_size;

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
void bt_op(VirtualCPU* cpu, int reg1, int bit_index);
void bset_op(VirtualCPU* cpu, int reg1, int bit_index);
void bclr_op(VirtualCPU* cpu, int reg1, int bit_index);
void btog_op(VirtualCPU* cpu, int reg1, int bit_index);
void strcmp_op(VirtualCPU* cpu, int reg_addr1, int reg_addr2);
void strlen_op(VirtualCPU* cpu, int reg_dest, int reg_addr);
void strcpy_op(VirtualCPU* cpu, int reg_dest_addr, int reg_src_addr);
void memcpy_op(VirtualCPU* cpu, int dest_addr_reg, int src_addr_reg, int len_reg);
void memset_op(VirtualCPU* cpu, int dest_addr_reg, int val_reg, int len_reg);
void cpuid_op(VirtualCPU* cpu, int dest_reg);

void audioCallback(void* userdata, Uint8* stream, int len);

bool isValidReg(int reg) {
    return reg >= 0 && reg < NUM_REGISTERS;
}
bool isValidMem(int addr) {
    return addr >= 0 && addr < MEMORY_SIZE;
}

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

bool initialize_disk_image(VirtualCPU* cpu) {
    if (cpu->disk_image_fp != NULL) {
        fclose(cpu->disk_image_fp);
        cpu->disk_image_fp = NULL;
    }

    FILE* fp = fopen(DISK_IMAGE_FILENAME, "rb+");

    if (fp == NULL) {
        printf("Disk image '%s' not found, creating new 16MB file...\n", DISK_IMAGE_FILENAME);
        fp = fopen(DISK_IMAGE_FILENAME, "wb+");
        if (fp == NULL) {
            perror("Error creating disk image file");
            fprintf(stderr, "Failed to create '%s'. Disk operations will fail.\n", DISK_IMAGE_FILENAME);
            return false;
        }

        char* zero_buffer = (char*)calloc(DISK_SECTOR_SIZE, 1);
        if (!zero_buffer) {
            fprintf(stderr, "Failed to allocate zero buffer for disk init.\n");
            fclose(fp);
            return false;
        }
        long long bytes_written = 0;
        size_t write_result;
        fseek(fp, 0, SEEK_SET);
        printf("Writing zeros to disk image (this may take a moment)...\n");
        while (bytes_written < DISK_IMAGE_SIZE_BYTES) {
            size_t bytes_to_write = DISK_SECTOR_SIZE;
            if (bytes_written + bytes_to_write > DISK_IMAGE_SIZE_BYTES) {
                bytes_to_write = DISK_IMAGE_SIZE_BYTES - bytes_written;
                if (bytes_to_write == 0) break;
            }
            write_result = fwrite(zero_buffer, 1, bytes_to_write, fp);
            if (write_result != bytes_to_write) {
                perror("Error writing zeros to new disk image file");
                fprintf(stderr, "Failed to fully initialize '%s'. Disk operations may fail.\n", DISK_IMAGE_FILENAME);
                free(zero_buffer);
                fclose(fp);
                return false;
            }
            bytes_written += write_result;
        }
        printf("Disk image created and zeroed.\n");
        free(zero_buffer);
        fflush(fp);
        fseek(fp, 0, SEEK_SET);
    }

    fseek(fp, 0, SEEK_END);
    long long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size < 0) {
        perror("Error getting disk image file size");
        fclose(fp);
        return false;
    }

    cpu->disk_image_fp = fp;
    cpu->disk_image_size = file_size;
    cpu->disk_sector_size = DISK_SECTOR_SIZE;

    printf("Disk image '%s' opened. Size: %lld bytes (%lld sectors).\n",
        DISK_IMAGE_FILENAME, cpu->disk_image_size, cpu->disk_image_size / cpu->disk_sector_size);

    if (cpu->disk_image_size != DISK_IMAGE_SIZE_BYTES) {
        fprintf(stderr, "Warning: Disk image size (%lld bytes) differs from expected (%d bytes).\n",
            cpu->disk_image_size, DISK_IMAGE_SIZE_BYTES);
    }


    return true;
}

bool init_cpu(VirtualCPU* cpu) {
    if (!cpu) return false;

    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->memory, 0, sizeof(cpu->memory));
    cpu->ip = 0;
    cpu->flags = 0;
    cpu->registers[15] = MEMORY_SIZE;
    cpu->shutdown_requested = false;

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

    for (int i = 0; i < MAX_OPEN_FILES; ++i) {
        cpu->open_files[i] = NULL;
        cpu->file_slot_used[i] = false;
    }

    cpu->disk_image_fp = NULL;
    cpu->disk_image_size = 0;
    cpu->disk_sector_size = 512;

    srand(time(NULL));

    return true;
}

void cleanup_cpu(VirtualCPU* cpu) {
    if (cpu) {
        free(cpu->pixels);
        cpu->pixels = NULL;

        for (int i = 0; i < MAX_OPEN_FILES; ++i) {
            if (cpu->open_files[i]) {
                fclose(cpu->open_files[i]);
                cpu->open_files[i] = NULL;
                cpu->file_slot_used[i] = false;
            }
        }

        if (cpu->disk_image_fp) {
            printf("Closing disk image '%s'.\n", DISK_IMAGE_FILENAME);
            fclose(cpu->disk_image_fp);
            cpu->disk_image_fp = NULL;
        }
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

#ifdef _WIN32
void enable_virtual_terminal_processing_if_needed() {
    static bool initialized = false;
    if (initialized) return;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return;
    }

    if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode)) {
            fprintf(stderr, "Warning: Failed to enable virtual terminal processing for console.\n");
        }
        else {
            initialized = true;
        }
    }
    else {
        initialized = true;
    }
}
#else
void enable_virtual_terminal_processing_if_needed() {}
#endif

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
    if (strcmp(instruction, "BT") == 0) return BT;
    if (strcmp(instruction, "BSET") == 0) return BSET;
    if (strcmp(instruction, "BCLR") == 0) return BCLR;
    if (strcmp(instruction, "BTOG") == 0) return BTOG;
    if (strcmp(instruction, "STRCMP") == 0) return STRCMP;
    if (strcmp(instruction, "STRLEN") == 0) return STRLEN;
    if (strcmp(instruction, "STRCPY") == 0) return STRCPY;
    if (strcmp(instruction, "MEMCPY") == 0) return MEMCPY;
    if (strcmp(instruction, "MEMSET") == 0) return MEMSET;
    if (strcmp(instruction, "CPUID") == 0) return CPUID;
    return INVALID_INST;
}

int loadProgram(const char* filename, char* program[], int max_size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open program file '%s'\n", filename);
        return -1;
    }

    char buffer[MAX_LINE_LENGTH];
    char** temp_program = (char**)malloc(max_size * sizeof(char*));
    if (!temp_program) {
        fprintf(stderr, "Error: Memory allocation failed for temp_program.\n");
        fclose(file);
        return -2;
    }
    memset(temp_program, 0, max_size * sizeof(char*));
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

        if (strncmp(start, "#warning", 8) == 0 && (isspace((unsigned char)start[8]) || start[8] == '\0')) {
            char* warning_msg = start + 8;
            while (isspace((unsigned char)*warning_msg)) warning_msg++;
            fprintf(stderr, "Warning: %s\n", warning_msg);
            continue;
        }

        if (strncmp(start, "#error", 6) == 0 && (isspace((unsigned char)start[6]) || start[6] == '\0')) {
            char* error_msg = start + 6;
            while (isspace((unsigned char)*error_msg)) error_msg++;
            fprintf(stderr, "Error: %s\n", error_msg);
            for (int i = 0; i < temp_line_count; ++i) {
                if (temp_program[i]) free(temp_program[i]);
            }
            free(temp_program);
            fclose(file);
            return -4;
        }

        if (strlen(start) == 0) {
            continue;
        }
        temp_program[temp_line_count] = strdup_portable(start);
        if (!temp_program[temp_line_count]) {
            fprintf(stderr, "Error: Memory allocation failed during program load.\n");
            for (int i = 0; i < temp_line_count; ++i) free(temp_program[i]);
            free(temp_program);
            fclose(file);
            return -2;
        }
        temp_line_count++;
    }
    fclose(file);

    if (temp_line_count >= max_size && fgets(buffer, sizeof(buffer), file) != NULL) {
        fprintf(stderr, "Error: Program exceeds maximum size of %d lines.\n", max_size);
        for (int i = 0; i < temp_line_count; ++i) free(temp_program[i]);
        free(temp_program);
        return -3;
    }

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
    LabelEntry* label_map = (LabelEntry*)malloc(max_size * sizeof(LabelEntry));
    if (!label_map) {
        fprintf(stderr, "Error: Memory allocation failed for label_map.\n");
        for (int i = 0; i < temp_line_count; ++i) if (temp_program[i]) free(temp_program[i]);
        free(temp_program);
        return -2;
    }
    int label_count = 0;
    int final_program_size = 0;

    for (int i = 0; i < temp_line_count; i++) {
        char* line = temp_program[i];
        if (!line) continue;

        char* colon_pos = NULL;
        bool in_quotes = false;

        for (char* p = line; *p; ++p) {
            if (*p == '"') {
                in_quotes = !in_quotes;
            }
            else if (*p == ':' && !in_quotes) {
                colon_pos = p;
                break;
            }
        }

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
                for (int j = i; j < temp_line_count; ++j) if (temp_program[j]) free(temp_program[j]);
                free(temp_program);
                free(label_map);
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

    free(temp_program);
    free(label_map);

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
    case 0x00:
        nop(cpu);
        break;
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

    case INT_GET_PIXEL:
    {
        int x = cpu->registers[0];
        int y = cpu->registers[1];
        int result_reg = 0;

        cpu->registers[result_reg] = -1;

        if (!cpu->pixels) {
            fprintf(stderr, "Error (INT 0x24): Pixel buffer not initialized.\n");
            break;
        }

        if (x >= 0 && x < cpu->screen_width && y >= 0 && y < cpu->screen_height) {
            Uint32 target_pixel = cpu->pixels[y * cpu->screen_width + x];
            for (int i = 0; i < PALETTE_SIZE; ++i) {
                SDL_Color p_color = cpu->palette[i];
                Uint32 palette_pixel = ((Uint32)p_color.a << 24) |
                    ((Uint32)p_color.r << 16) |
                    ((Uint32)p_color.g << 8) |
                    ((Uint32)p_color.b);
                if (target_pixel == palette_pixel) {
                    cpu->registers[result_reg] = i;
                    break;
                }
            }
        }
        else {
            fprintf(stderr, "Warning (INT 0x24): Coordinates (%d, %d) out of bounds (%dx%d).\n", x, y, cpu->screen_width, cpu->screen_height);
        }
    }
    break;

    case INT_GET_KEY_STATE:
    {
        int scancode_reg = 0;
        int result_reg = 0;
        SDL_Scancode scancode = (SDL_Scancode)cpu->registers[scancode_reg];

        SDL_PumpEvents();

        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        if (keystate == NULL) {
            fprintf(stderr, "Error (INT 0x25): SDL_GetKeyboardState returned NULL.\n");
            cpu->registers[result_reg] = 0;
            break;
        }

        if (scancode >= 0 && scancode < SDL_NUM_SCANCODES) {
            cpu->registers[result_reg] = keystate[scancode] ? 1 : 0;
        }
        else {
            cpu->registers[result_reg] = 0;
        }
    }
    break;

    case INT_WAIT_FOR_KEY:
    {
        int result_reg = 0;
        SDL_Event event;
        bool key_pressed = false;

        while (!key_pressed) {
            if (SDL_WaitEventTimeout(&event, 10)) {
                if (event.type == SDL_QUIT) {
                    fprintf(stdout, "SDL_QUIT received during INT 0x26.\n");
                    cpu->registers[result_reg] = -1;
                    key_pressed = true;
                }
                else if (event.type == SDL_KEYDOWN) {
                    cpu->registers[result_reg] = event.key.keysym.scancode;
                    key_pressed = true;
                }
            }
        }
    }
    break;

    case INT_GET_MOUSE_STATE:
    {
        int x_reg = 0;
        int y_reg = 1;
        int button_reg = 2;
        int mouse_x, mouse_y;

        SDL_PumpEvents();

        Uint32 button_state = SDL_GetMouseState(&mouse_x, &mouse_y);

        cpu->registers[x_reg] = mouse_x;
        cpu->registers[y_reg] = mouse_y;

        int button_mask = 0;
        if (button_state & SDL_BUTTON(SDL_BUTTON_LEFT))   button_mask |= 1;
        if (button_state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) button_mask |= 2;
        if (button_state & SDL_BUTTON(SDL_BUTTON_RIGHT))  button_mask |= 4;

        cpu->registers[button_reg] = button_mask;
    }
    break;

    case INT_GET_MEMORY_SIZE:
    {
        cpu->registers[0] = MEMORY_SIZE;
    }
    break;

    case INT_FILE_OPEN:
    {
        int name_addr_reg = 0;
        int mode_addr_reg = 1;
        int result_reg = 0;

        int name_addr = cpu->registers[name_addr_reg];
        int mode_addr = cpu->registers[mode_addr_reg];

        cpu->registers[result_reg] = -1;

        if (!isValidMem(name_addr) || !isValidMem(mode_addr)) {
            fprintf(stderr, "Error (INT 0x29): Invalid memory address for filename (0x%X) or mode (0x%X).\n", name_addr, mode_addr);
            break;
        }

        int handle = -1;
        for (int i = 0; i < MAX_OPEN_FILES; ++i) {
            if (!cpu->file_slot_used[i]) {
                handle = i;
                break;
            }
        }

        if (handle == -1) {
            fprintf(stderr, "Error (INT 0x29): Maximum open files (%d) reached.\n", MAX_OPEN_FILES);
            break;
        }

        char filename_buf[FILENAME_MAX];
        char mode_buf[8];
        int fn_idx = 0;
        int md_idx = 0;

        while (isValidMem(name_addr + fn_idx) && fn_idx < FILENAME_MAX - 1) {
            char c = (char)cpu->memory[name_addr + fn_idx];
            filename_buf[fn_idx] = c;
            if (c == 0) break;
            fn_idx++;
        }
        filename_buf[fn_idx] = 0;

        while (isValidMem(mode_addr + md_idx) && md_idx < sizeof(mode_buf) - 1) {
            char c = (char)cpu->memory[mode_addr + md_idx];
            mode_buf[md_idx] = c;
            if (c == 0) break;
            md_idx++;
        }
        mode_buf[md_idx] = 0;


        if (fn_idx == FILENAME_MAX - 1 && filename_buf[fn_idx] != 0) {
            fprintf(stderr, "Error (INT 0x29): Filename string too long or not null-terminated.\n");
            break;
        }
        if (md_idx == sizeof(mode_buf) - 1 && mode_buf[md_idx] != 0) {
            fprintf(stderr, "Error (INT 0x29): Mode string too long or not null-terminated.\n");
            break;
        }


        FILE* fp = fopen(filename_buf, mode_buf);
        if (fp) {
            cpu->open_files[handle] = fp;
            cpu->file_slot_used[handle] = true;
            cpu->registers[result_reg] = handle;
        }
        else {
            fprintf(stderr, "Error (INT 0x29): Failed to open file '%s' with mode '%s'.\n", filename_buf, mode_buf);
            perror(" fopen");
        }
    }
    break;

    case INT_FILE_CLOSE:
    {
        int handle_reg = 0;
        int result_reg = 0;
        int handle = cpu->registers[handle_reg];

        cpu->registers[result_reg] = -1;

        if (handle >= 0 && handle < MAX_OPEN_FILES && cpu->file_slot_used[handle] && cpu->open_files[handle]) {
            int close_result = fclose(cpu->open_files[handle]);
            cpu->open_files[handle] = NULL;
            cpu->file_slot_used[handle] = false;
            if (close_result == 0) {
                cpu->registers[result_reg] = 0;
            }
            else {
                fprintf(stderr, "Error (INT 0x2A): fclose failed for handle %d.\n", handle);
                perror(" fclose");
            }
        }
        else {
            fprintf(stderr, "Error (INT 0x2A): Invalid or already closed file handle %d.\n", handle);
        }
    }
    break;

    case INT_FILE_READ:
    {
        int handle_reg = 0;
        int mem_addr_reg = 1;
        int bytes_reg = 2;
        int result_reg = 0;

        int handle = cpu->registers[handle_reg];
        int mem_addr = cpu->registers[mem_addr_reg];
        int bytes_to_read = cpu->registers[bytes_reg];

        cpu->registers[result_reg] = -1;

        if (handle < 0 || handle >= MAX_OPEN_FILES || !cpu->file_slot_used[handle] || !cpu->open_files[handle]) {
            fprintf(stderr, "Error (INT 0x2B): Invalid file handle %d.\n", handle);
            break;
        }
        if (bytes_to_read <= 0) {
            cpu->registers[result_reg] = 0;
            break;
        }
        if (!isValidMem(mem_addr) || !isValidMem(mem_addr + bytes_to_read - 1)) {
            fprintf(stderr, "Error (INT 0x2B): Invalid memory range 0x%X - 0x%X for reading %d bytes.\n", mem_addr, mem_addr + bytes_to_read - 1, bytes_to_read);
            break;
        }

        size_t buf_size = (size_t)bytes_to_read;
        unsigned char* temp_buf = malloc(buf_size);
        if (!temp_buf) {
            fprintf(stderr, "Error (INT 0x2B): Failed to allocate temporary read buffer (%zu bytes).\n", buf_size);
            break;
        }

        size_t bytes_read = fread(temp_buf, 1, buf_size, cpu->open_files[handle]);

        if (bytes_read > 0) {
            for (size_t i = 0; i < bytes_read; ++i) {
                if (isValidMem(mem_addr + i)) {
                    cpu->memory[mem_addr + i] = temp_buf[i];
                }
                else {
                    fprintf(stderr, "FATAL Error (INT 0x2B): Memory bounds exceeded during copy after fread! This shouldn't happen.\n");
                    bytes_read = i;
                    break;
                }
            }
        }
        else {
            if (ferror(cpu->open_files[handle])) {
                fprintf(stderr, "Error (INT 0x2B): fread failed for handle %d.\n", handle);
                perror(" fread");
                bytes_read = (size_t)-1;
            }
            else if (feof(cpu->open_files[handle])) {
                // EOF handled by bytes_read == 0
            }
        }

        free(temp_buf);
        cpu->registers[result_reg] = (int)bytes_read;

    }
    break;

    case INT_FILE_WRITE:
    {
        int handle_reg = 0;
        int mem_addr_reg = 1;
        int bytes_reg = 2;
        int result_reg = 0;

        int handle = cpu->registers[handle_reg];
        int mem_addr = cpu->registers[mem_addr_reg];
        int bytes_to_write = cpu->registers[bytes_reg];

        cpu->registers[result_reg] = -1;

        if (handle < 0 || handle >= MAX_OPEN_FILES || !cpu->file_slot_used[handle] || !cpu->open_files[handle]) {
            fprintf(stderr, "Error (INT 0x2C): Invalid file handle %d.\n", handle);
            break;
        }
        if (bytes_to_write < 0) {
            fprintf(stderr, "Error (INT 0x2C): Attempted to write negative bytes (%d).\n", bytes_to_write);
            break;
        }
        if (bytes_to_write > 0 && (!isValidMem(mem_addr) || !isValidMem(mem_addr + bytes_to_write - 1))) {
            fprintf(stderr, "Error (INT 0x2C): Invalid memory range 0x%X - 0x%X for writing %d bytes.\n", mem_addr, mem_addr + bytes_to_write - 1, bytes_to_write);
            break;
        }
        if (bytes_to_write == 0) {
            cpu->registers[result_reg] = 0;
            break;
        }


        size_t buf_size = (size_t)bytes_to_write;
        unsigned char* temp_buf = malloc(buf_size);
        if (!temp_buf) {
            fprintf(stderr, "Error (INT 0x2C): Failed to allocate temporary write buffer (%zu bytes).\n", buf_size);
            break;
        }

        for (size_t i = 0; i < buf_size; ++i) {
            if (isValidMem(mem_addr + i)) {
                temp_buf[i] = (unsigned char)(cpu->memory[mem_addr + i] & 0xFF);
            }
            else {
                fprintf(stderr, "FATAL Error (INT 0x2C): Memory bounds exceeded during copy before fwrite! This shouldn't happen.\n");
                free(temp_buf);
                goto file_write_end;
            }
        }

        size_t bytes_written = fwrite(temp_buf, 1, buf_size, cpu->open_files[handle]);

        if (bytes_written != buf_size) {
            fprintf(stderr, "Error (INT 0x2C): fwrite failed or wrote partial data for handle %d (wrote %zu of %zu).\n", handle, bytes_written, buf_size);
            perror(" fwrite");
        }

        free(temp_buf);
        cpu->registers[result_reg] = (int)bytes_written;

    file_write_end:;
    }
    break;

    case INT_FILE_SEEK:
    {
        int handle_reg = 0;
        int offset_reg = 1;
        int origin_reg = 2;
        int result_reg = 0;

        int handle = cpu->registers[handle_reg];
        long offset = (long)cpu->registers[offset_reg];
        int origin_code = cpu->registers[origin_reg];

        cpu->registers[result_reg] = -1;

        if (handle < 0 || handle >= MAX_OPEN_FILES || !cpu->file_slot_used[handle] || !cpu->open_files[handle]) {
            fprintf(stderr, "Error (INT 0x2D): Invalid file handle %d.\n", handle);
            break;
        }

        int seek_origin;
        switch (origin_code) {
        case 0: seek_origin = SEEK_SET; break;
        case 1: seek_origin = SEEK_CUR; break;
        case 2: seek_origin = SEEK_END; break;
        default:
            fprintf(stderr, "Error (INT 0x2D): Invalid origin code %d (must be 0, 1, or 2).\n", origin_code);
            break;
        }

        int seek_result = fseek(cpu->open_files[handle], offset, seek_origin);

        if (seek_result == 0) {
            cpu->registers[result_reg] = 0;
        }
        else {
            fprintf(stderr, "Error (INT 0x2D): fseek failed for handle %d.\n", handle);
            perror(" fseek");
        }
    }
    break;

    case INT_FILE_TELL:
    {
        int handle_reg = 0;
        int result_reg = 0;
        int handle = cpu->registers[handle_reg];

        cpu->registers[result_reg] = -1;

        if (handle < 0 || handle >= MAX_OPEN_FILES || !cpu->file_slot_used[handle] || !cpu->open_files[handle]) {
            fprintf(stderr, "Error (INT 0x2E): Invalid file handle %d.\n", handle);
            break;
        }

        long current_pos = ftell(cpu->open_files[handle]);

        if (current_pos == -1L) {
            fprintf(stderr, "Error (INT 0x2E): ftell failed for handle %d.\n", handle);
            perror(" ftell");
        }
        else {
            if (current_pos > INT_MAX || current_pos < INT_MIN) {
                fprintf(stderr, "Warning (INT 0x2E): File position %ld exceeds integer limits for handle %d.\n", current_pos, handle);
            }
            else {
                cpu->registers[result_reg] = (int)current_pos;
            }
        }
    }
    break;
    case INT_SET_CONSOLE_COLOR: // 0x2F
    {
        int fg_color = cpu->registers[0];
        int bg_color = cpu->registers[1];

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            fg_color &= 0x0F;
            bg_color &= 0x0F;
            WORD attribute = (WORD)((bg_color << 4) | fg_color);
            SetConsoleTextAttribute(hConsole, attribute);
        }
        else {
            fprintf(stderr, "Warning (INT 0x2F): Could not get console handle (Win32).\n");
        }
#else // POSIX/ANSI
        int ansi_fg, ansi_bg;
        if (fg_color >= 0 && fg_color <= 7) ansi_fg = 30 + fg_color;
        else if (fg_color >= 8 && fg_color <= 15) ansi_fg = 90 + (fg_color - 8);
        else ansi_fg = 39;

        if (bg_color >= 0 && bg_color <= 7) ansi_bg = 40 + bg_color;
        else if (bg_color >= 8 && bg_color <= 15) ansi_bg = 100 + (bg_color - 8);
        else ansi_bg = 49; 

        printf("\033[%d;%dm", ansi_fg, ansi_bg);
        fflush(stdout);
#endif
    }
    break;

    case INT_RESET_CONSOLE_COLOR:
    {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        WORD saved_attributes = 7;
        if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
            saved_attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(hConsole, saved_attributes);
        }
        else {
            fprintf(stderr, "Warning (INT 0x30): Could not get console handle (Win32).\n");
        }
#else // POSIX/ANSI
        printf("\033[0m");
        fflush(stdout);
#endif
    }
    break;

    case INT_GOTOXY:
    {
        int col = cpu->registers[0];
        int row = cpu->registers[1];

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            COORD coord = { (SHORT)col, (SHORT)row };
            if (coord.X < 0) coord.X = 0;
            if (coord.Y < 0) coord.Y = 0;
            SetConsoleCursorPosition(hConsole, coord);
        }
        else {
            fprintf(stderr, "Warning (INT 0x31): Could not get console handle (Win32).\n");
        }
#else // POSIX/ANSI
        printf("\033[%d;%dH", row + 1, col + 1);
        fflush(stdout);
#endif
    }
    break;

    case INT_GETXY:
    {
        int col_reg = 0;
        int row_reg = 1;

        cpu->registers[col_reg] = -1;
        cpu->registers[row_reg] = -1;

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
            cpu->registers[col_reg] = consoleInfo.dwCursorPosition.X;
            cpu->registers[row_reg] = consoleInfo.dwCursorPosition.Y;
        }
        else {
            fprintf(stderr, "Warning (INT 0x32): Failed to get console buffer info (Win32).\n");
        }
#else // POSIX/ANSI
        fprintf(stderr, "Warning (INT 0x32): INT_GETXY is not reliably implemented on non-Windows platforms.\n");
#endif
    }
    break;

    case INT_GET_CONSOLE_SIZE:
    {
        int width_reg = 0;
        int height_reg = 1;

        cpu->registers[width_reg] = -1;
        cpu->registers[height_reg] = -1;

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
            cpu->registers[width_reg] = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
            cpu->registers[height_reg] = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
        }
        else {
            fprintf(stderr, "Warning (INT 0x33): Failed to get console buffer info (Win32).\n");
        }
#else // POSIX
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
            cpu->registers[width_reg] = ws.ws_col;
            cpu->registers[height_reg] = ws.ws_row;
        }
        else {
            perror("ioctl TIOCGWINSZ");
            fprintf(stderr, "Warning (INT 0x33): Failed to get console size (ioctl).\n");
        }
#endif
    }
    break;

    case INT_SET_CONSOLE_TITLE:
    {
        int title_addr_reg = 0;
        int title_addr = cpu->registers[title_addr_reg];

        if (!isValidMem(title_addr)) {
            fprintf(stderr, "Error (INT 0x34): Invalid memory address for title string (0x%X).\n", title_addr);
            break;
        }

        char title_buf[256];
        int idx = 0;
        while (isValidMem(title_addr + idx) && idx < sizeof(title_buf) - 1) {
            char c = (char)cpu->memory[title_addr + idx];
            title_buf[idx] = c;
            if (c == 0) break;
            idx++;
        }
        title_buf[idx] = 0;

        if (idx == sizeof(title_buf) - 1 && title_buf[idx] != 0) {
            fprintf(stderr, "Warning (INT 0x34): Title string too long or not null-terminated.\n");
        }

#ifdef _WIN32
        if (!SetConsoleTitle(title_buf)) {
            fprintf(stderr, "Warning (INT 0x34): SetConsoleTitle failed (Win32).\n");
        }
#else // POSIX/ANSI
        printf("\033]0;%s\007", title_buf);
        fflush(stdout);
#endif
    }
    break;

    case INT_SYSTEM_SHUTDOWN:
        cpu->shutdown_requested = true;
        exit(0);
        break;

    case INT_GET_DATETIME:
    {
        time_t timer;
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);

        cpu->registers[0] = tm_info->tm_year + 1900;
        cpu->registers[1] = tm_info->tm_mon + 1;
        cpu->registers[2] = tm_info->tm_mday;
        cpu->registers[3] = tm_info->tm_hour;
        cpu->registers[4] = tm_info->tm_min;
        cpu->registers[5] = tm_info->tm_sec;
    }
    break;

    case INT_DISK_READ:
    {
        int sector_num_reg = 0;
        int mem_addr_reg = 1;
        int num_sectors_reg = 2;
        int status_reg = 0;

        int sector_num = cpu->registers[sector_num_reg];
        int mem_addr = cpu->registers[mem_addr_reg];
        int num_sectors = cpu->registers[num_sectors_reg];
        int status = 0;

        if (!cpu->disk_image_fp) {
            fprintf(stderr, "Error (INT 0x37): Disk image not open.\n");
            status = 3;
            goto disk_read_end;
        }
        if (num_sectors <= 0) {
            status = 0;
            goto disk_read_end;
        }

        long long max_sector = (cpu->disk_image_size / cpu->disk_sector_size) - 1;
        if (sector_num < 0 || sector_num > max_sector || (sector_num + num_sectors - 1) > max_sector) {
            fprintf(stderr, "Error (INT 0x37): Invalid sector range %d to %d (max %lld).\n",
                sector_num, sector_num + num_sectors - 1, max_sector);
            status = 1;
            goto disk_read_end;
        }

        long long total_bytes = (long long)num_sectors * cpu->disk_sector_size;
        if (!isValidMem(mem_addr) || !isValidMem(mem_addr + total_bytes - 1)) {
            fprintf(stderr, "Error (INT 0x37): Invalid memory range 0x%X to 0x%X for %lld bytes.\n",
                mem_addr, mem_addr + (int)total_bytes - 1, total_bytes);
            status = 2;
            goto disk_read_end;
        }

        long long seek_pos = (long long)sector_num * cpu->disk_sector_size;
        if (fseek(cpu->disk_image_fp, seek_pos, SEEK_SET) != 0) {
            perror("Error (INT 0x37): fseek failed");
            status = 3;
            goto disk_read_end;
        }

        char* sector_buffer = malloc(total_bytes);
        if (!sector_buffer) {
            fprintf(stderr, "Error (INT 0x37): Failed to allocate sector buffer (%lld bytes).\n", total_bytes);
            status = 3;
            goto disk_read_end;
        }

        size_t bytes_read = fread(sector_buffer, 1, total_bytes, cpu->disk_image_fp);
        if (bytes_read != (size_t)total_bytes) {
            if (ferror(cpu->disk_image_fp)) {
                perror("Error (INT 0x37): fread failed");
            }
            else {
                fprintf(stderr, "Error (INT 0x37): fread returned short count (%zu != %lld).\n", bytes_read, total_bytes);
            }
            free(sector_buffer);
            status = 3;
            goto disk_read_end;
        }

        for (long long i = 0; i < total_bytes; ++i) {
            if (isValidMem(mem_addr + i)) {
                cpu->memory[mem_addr + i] = (int)(unsigned char)sector_buffer[i];
            }
            else {
                fprintf(stderr, "FATAL Error (INT 0x37): Memory bounds exceeded during copy after fread!.\n");
                status = 2;
                break;
            }
        }
        free(sector_buffer);

    disk_read_end:
        cpu->registers[status_reg] = status;
    }
    break;

    case INT_DISK_WRITE:
    {
        int sector_num_reg = 0;
        int mem_addr_reg = 1;
        int num_sectors_reg = 2;
        int status_reg = 0;

        int sector_num = cpu->registers[sector_num_reg];
        int mem_addr = cpu->registers[mem_addr_reg];
        int num_sectors = cpu->registers[num_sectors_reg];
        int status = 0;

        if (!cpu->disk_image_fp) {
            fprintf(stderr, "Error (INT 0x38): Disk image not open.\n");
            status = 3;
            goto disk_write_end;
        }
        if (num_sectors <= 0) {
            status = 0;
            goto disk_write_end;
        }

        long long max_sector = (cpu->disk_image_size / cpu->disk_sector_size) - 1;
        if (sector_num < 0 || sector_num > max_sector || (sector_num + num_sectors - 1) > max_sector) {
            fprintf(stderr, "Error (INT 0x38): Invalid sector range %d to %d (max %lld).\n",
                sector_num, sector_num + num_sectors - 1, max_sector);
            status = 1;
            goto disk_write_end;
        }

        long long total_bytes = (long long)num_sectors * cpu->disk_sector_size;
        if (!isValidMem(mem_addr) || !isValidMem(mem_addr + total_bytes - 1)) {
            fprintf(stderr, "Error (INT 0x38): Invalid memory range 0x%X to 0x%X for %lld bytes.\n",
                mem_addr, mem_addr + (int)total_bytes - 1, total_bytes);
            status = 2;
            goto disk_write_end;
        }

        char* sector_buffer = malloc(total_bytes);
        if (!sector_buffer) {
            fprintf(stderr, "Error (INT 0x38): Failed to allocate sector buffer (%lld bytes).\n", total_bytes);
            status = 3;
            goto disk_write_end;
        }

        for (long long i = 0; i < total_bytes; ++i) {
            if (isValidMem(mem_addr + i)) {
                sector_buffer[i] = (char)(cpu->memory[mem_addr + i] & 0xFF);
            }
            else {
                fprintf(stderr, "FATAL Error (INT 0x38): Memory bounds exceeded during copy before fwrite!.\n");
                free(sector_buffer);
                status = 2;
                goto disk_write_end;
            }
        }

        long long seek_pos = (long long)sector_num * cpu->disk_sector_size;
        if (fseek(cpu->disk_image_fp, seek_pos, SEEK_SET) != 0) {
            perror("Error (INT 0x38): fseek failed");
            free(sector_buffer);
            status = 3;
            goto disk_write_end;
        }

        size_t bytes_written = fwrite(sector_buffer, 1, total_bytes, cpu->disk_image_fp);
        free(sector_buffer);

        if (bytes_written != (size_t)total_bytes) {
            if (ferror(cpu->disk_image_fp)) {
                perror("Error (INT 0x38): fwrite failed");
            }
            else {
                fprintf(stderr, "Error (INT 0x38): fwrite wrote short count (%zu != %lld).\n", bytes_written, total_bytes);
            }
            status = 3;
            fflush(cpu->disk_image_fp);
            goto disk_write_end;
        }

        if (fflush(cpu->disk_image_fp) != 0) {
            perror("Error (INT 0x38): fflush failed after write");
            status = 3;
            goto disk_write_end;
        }

    disk_write_end:
        cpu->registers[status_reg] = status;
    }
    break;

    case INT_DISK_INFO:
    {
        int total_sectors_reg = 0;
        int sector_size_reg = 1;

        if (!cpu->disk_image_fp) {
            fprintf(stderr, "Error (INT 0x39): Disk image not open.\n");
            cpu->registers[total_sectors_reg] = -1;
            cpu->registers[sector_size_reg] = 0;
        }
        else {
            long long total_sectors = cpu->disk_image_size / cpu->disk_sector_size;
            if (total_sectors > INT_MAX) {
                fprintf(stderr, "Warning (INT 0x39): Total sector count (%lld) exceeds INT_MAX.\n", total_sectors);
                cpu->registers[total_sectors_reg] = INT_MAX;
            }
            else {
                cpu->registers[total_sectors_reg] = (int)total_sectors;
            }
            cpu->registers[sector_size_reg] = cpu->disk_sector_size;
        }
    }
    break;
    case INT_BREAKPOINT:
#ifdef _MSC_VER
        __debugbreak();
#else
        __builtin_trap();
#endif
        break;

    default:
        fprintf(stderr, "Error: Unknown interrupt code 0x%X at line %d\n", interrupt_id, cpu->ip + 1);
        break;
    }
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
                bool parsed_operand2 = false;

                if (operand2_str[0] == 'R' && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error MOV: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                    }
                    else {
                        cpu->registers[dest_reg] = cpu->registers[src_reg];
                        parsed_operand2 = true;
                    }
                }
                else if ((operand2_str[0] == '0' && (operand2_str[1] == 'x' || operand2_str[1] == 'X')) &&
                    sscanf(operand2_str, "%x", &immediate_val) == 1) {
                    mov(cpu, dest_reg, immediate_val);
                    parsed_operand2 = true;
                }
                else if (isdigit((unsigned char)operand2_str[0]) || (operand2_str[0] == '-' && isdigit((unsigned char)operand2_str[1]))) {
                    if (sscanf(operand2_str, "%d", &immediate_val) == 1) {
                        mov(cpu, dest_reg, immediate_val);
                        parsed_operand2 = true;
                    }
                }

                if (!parsed_operand2) {
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
                // Manually parse STRMOV arguments for robustness
                char* args_start = strchr(current_instruction_line, ' ');
                if (!args_start) {
                    fprintf(stderr, "Error: Invalid STRMOV format (no arguments) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                    running = false;
                    break;
                }
                args_start++;

                char* end_ptr_addr;
                long parsed_addr = strtol(args_start, &end_ptr_addr, 10);
                if (end_ptr_addr == args_start) {
                    fprintf(stderr, "Error: Invalid STRMOV format (cannot parse address) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                    running = false;
                    break;
                }
                int addr = (int)parsed_addr;

                char* comma_ptr = strchr(end_ptr_addr, ',');
                if (!comma_ptr) {
                    fprintf(stderr, "Error: Invalid STRMOV format (missing comma) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                    running = false;
                    break;
                }

                char* quote_start = strchr(comma_ptr + 1, '"');
                if (!quote_start) {
                    fprintf(stderr, "Error: Invalid STRMOV format (missing opening quote) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                    running = false;
                    break;
                }
                quote_start++;

                char* quote_end = strchr(quote_start, '"');
                if (!quote_end) {
                    fprintf(stderr, "Error: Invalid STRMOV format (missing closing quote) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                    running = false;
                    break;
                }

                size_t str_len = quote_end - quote_start;

                if (addr >= 0 && addr < MEMORY_SIZE) {
                    if (addr + str_len >= MEMORY_SIZE) {
                         fprintf(stderr, "Error: STRMOV destination out of bounds (addr=%d, len=%zu) at line %d\n", addr, str_len, cpu->ip + 1);
                         running = false;
                         break;
                    }

                    for (size_t i = 0; i < str_len; i++) {
                        if (quote_start + i >= current_instruction_line + strlen(current_instruction_line)){
                            fprintf(stderr, "Error: Internal parsing error reading STRMOV string at line %d\n", cpu->ip+1);
                            running = false;
                            break;
                        }
                        cpu->memory[addr + i] = *(quote_start + i);
                    }
                    if (!running) break;

                    cpu->memory[addr + str_len] = 0;

                } else {
                    fprintf(stderr, "Error: Invalid STRMOV memory address %d at line %d\n", addr, cpu->ip + 1);
                    running = false;
                }
            } break;
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
        case BT:
        case BSET:
        case BCLR:
        case BTOG:
        {
            int dest_reg = -1;
            char operand2_str[MAX_LINE_LENGTH];
            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error %s: Invalid destination register R%d at line %d\n", op_str, dest_reg, cpu->ip + 1);
                    break;
                }

                int bit_source_reg = -1;
                int immediate_bit_index = -1;
                bool immediate_mode = true;

                if (operand2_str[0] == 'R' && sscanf(operand2_str, "R%d", &bit_source_reg) == 1) {
                    if (!isValidReg(bit_source_reg)) {
                        fprintf(stderr, "Error %s: Invalid bit source register %s at line %d\n", op_str, operand2_str, cpu->ip + 1);
                        break;
                    }
                    immediate_mode = false;
                    immediate_bit_index = cpu->registers[bit_source_reg];
                }
                else if (sscanf(operand2_str, "%d", &immediate_bit_index) == 1) {
                    immediate_mode = true;
                }
                else {
                    fprintf(stderr, "Error %s: Invalid second operand '%s' at line %d\n", op_str, operand2_str, cpu->ip + 1);
                    break;
                }

                if (immediate_bit_index < 0 || immediate_bit_index >= 32) {
                    fprintf(stderr, "Error %s: Invalid bit index %d (must be 0-31) at line %d\n", op_str, immediate_bit_index, cpu->ip + 1);
                    break;
                }

                switch (inst) {
                case BT:   bt_op(cpu, dest_reg, immediate_bit_index); break;
                case BSET: bset_op(cpu, dest_reg, immediate_bit_index); break;
                case BCLR: bclr_op(cpu, dest_reg, immediate_bit_index); break;
                case BTOG: btog_op(cpu, dest_reg, immediate_bit_index); break;
                default:
                    fprintf(stderr, "Error: Reached bit op handler with invalid instruction %d\n", inst);
                    break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid %s format structure at line %d: '%s'\n", op_str, cpu->ip + 1, current_instruction_line);
            }
            break;
        }
        case STRCMP:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                strcmp_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid STRCMP format at line %d (Expected: STRCMP Raddr1, Raddr2)\n", cpu->ip + 1);
            }
            break;

        case STRLEN:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                strlen_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid STRLEN format at line %d (Expected: STRLEN Rdest, Raddr)\n", cpu->ip + 1);
            }
            break;

        case STRCPY:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                strcpy_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid STRCPY format at line %d (Expected: STRCPY Rdest_addr, Rsrc_addr)\n", cpu->ip + 1);
            }
            break;
        case MEMCPY:
            if (sscanf(current_instruction_line, "%*s R%d, R%d, R%d", &operands[0], &operands[1], &operands[2]) == 3) {
                memcpy_op(cpu, operands[0], operands[1], operands[2]);
            }
            else {
                fprintf(stderr, "Error: Invalid MEMCPY format at line %d (Expected: MEMCPY Rdest_addr, Rsrc_addr, Rlen)\n", cpu->ip + 1);
            }
            break;

        case MEMSET:
            if (sscanf(current_instruction_line, "%*s R%d, R%d, R%d", &operands[0], &operands[1], &operands[2]) == 3) {
                memset_op(cpu, operands[0], operands[1], operands[2]);
            }
            else {
                fprintf(stderr, "Error: Invalid MEMSET format at line %d (Expected: MEMSET Rdest_addr, Rval, Rlen)\n", cpu->ip + 1);
            }
            break;

        case CPUID:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                cpuid_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid CPUID format at line %d (Expected: CPUID Rdest)\n", cpu->ip + 1);
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

    if (cpu->ip >= program_size) {
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
    for (;;){
    }
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

void bt_op(VirtualCPU* cpu, int reg1, int bit_index) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error BT: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (bit_index < 0 || bit_index >= 32) {
        fprintf(stderr, "Error BT: Invalid bit index %d at line %d\n", bit_index, cpu->ip + 1);
        return;
    }

    unsigned int mask = 1U << bit_index;

    cpu->flags &= ~FLAG_ZERO;

    if ((cpu->registers[reg1] & mask) == 0) {
        cpu->flags |= FLAG_ZERO;
    }
}

void bset_op(VirtualCPU* cpu, int reg1, int bit_index) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error BSET: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (bit_index < 0 || bit_index >= 32) {
        fprintf(stderr, "Error BSET: Invalid bit index %d at line %d\n", bit_index, cpu->ip + 1);
        return;
    }

    unsigned int mask = 1U << bit_index;
    cpu->registers[reg1] |= mask;
}

void bclr_op(VirtualCPU* cpu, int reg1, int bit_index) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error BCLR: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (bit_index < 0 || bit_index >= 32) {
        fprintf(stderr, "Error BCLR: Invalid bit index %d at line %d\n", bit_index, cpu->ip + 1);
        return;
    }

    unsigned int mask = 1U << bit_index;
    cpu->registers[reg1] &= ~mask;
}

void btog_op(VirtualCPU* cpu, int reg1, int bit_index) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error BTOG: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (bit_index < 0 || bit_index >= 32) {
        fprintf(stderr, "Error BTOG: Invalid bit index %d at line %d\n", bit_index, cpu->ip + 1);
        return;
    }

    unsigned int mask = 1U << bit_index;
    cpu->registers[reg1] ^= mask;
}

void strcmp_op(VirtualCPU* cpu, int reg_addr1, int reg_addr2) {
    if (!isValidReg(reg_addr1) || !isValidReg(reg_addr2)) {
        fprintf(stderr, "Error STRCMP: Invalid register R%d or R%d at line %d\n", reg_addr1, reg_addr2, cpu->ip + 1);
        return;
    }

    int addr1 = cpu->registers[reg_addr1];
    int addr2 = cpu->registers[reg_addr2];

    if (!isValidMem(addr1) || !isValidMem(addr2)) {
        fprintf(stderr, "Error STRCMP: Invalid memory address 0x%X or 0x%X at line %d\n", addr1, addr2, cpu->ip + 1);
        return;
    }

    int char1, char2;
    int offset = 0;
    while (true) {
        int current_addr1 = addr1 + offset;
        int current_addr2 = addr2 + offset;

        if (!isValidMem(current_addr1) || !isValidMem(current_addr2)) {
            fprintf(stderr, "Error STRCMP: Memory access out of bounds during compare at line %d\n", cpu->ip + 1);
            cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS);
            cpu->flags |= FLAG_LESS;
            return;
        }

        char1 = cpu->memory[current_addr1];
        char2 = cpu->memory[current_addr2];

        if (char1 == 0 && char2 == 0) {
            cpu->flags &= ~(FLAG_GREATER | FLAG_LESS);
            cpu->flags |= FLAG_ZERO;
            return;
        }
        if (char1 != char2) {
            cpu->flags &= ~FLAG_ZERO;
            if (char1 > char2) {
                cpu->flags |= FLAG_GREATER;
                cpu->flags &= ~FLAG_LESS;
            }
            else {
                cpu->flags |= FLAG_LESS;
                cpu->flags &= ~FLAG_GREATER;
            }
            return;
        }
        offset++;
        if (offset >= MEMORY_SIZE) {
            fprintf(stderr, "Warning STRCMP: Comparison exceeded safety limit at line %d\n", cpu->ip + 1);
            cpu->flags &= ~FLAG_ZERO;
            cpu->flags |= FLAG_LESS;
            cpu->flags &= ~FLAG_GREATER;
            return;
        }
    }
}

void strlen_op(VirtualCPU* cpu, int reg_dest, int reg_addr) {
    if (!isValidReg(reg_dest) || !isValidReg(reg_addr)) {
        fprintf(stderr, "Error STRLEN: Invalid register R%d or R%d at line %d\n", reg_dest, reg_addr, cpu->ip + 1);
        return;
    }

    int addr = cpu->registers[reg_addr];
    if (!isValidMem(addr)) {
        fprintf(stderr, "Error STRLEN: Invalid memory address 0x%X in R%d at line %d\n", addr, reg_addr, cpu->ip + 1);
        cpu->registers[reg_dest] = -1;
        return;
    }

    int length = 0;
    int current_addr = addr;
    while (isValidMem(current_addr) && cpu->memory[current_addr] != 0) {
        length++;
        current_addr++;
        if (length >= MEMORY_SIZE) {
            fprintf(stderr, "Warning STRLEN: Length calculation exceeded safety limit at line %d\n", cpu->ip + 1);
            break;
        }
    }

    cpu->registers[reg_dest] = length;

    cpu->flags &= ~FLAG_ZERO;
    if (length == 0) {
        cpu->flags |= FLAG_ZERO;
    }
}

void strcpy_op(VirtualCPU* cpu, int reg_dest_addr, int reg_src_addr) {
    if (!isValidReg(reg_dest_addr) || !isValidReg(reg_src_addr)) {
        fprintf(stderr, "Error STRCPY: Invalid register R%d or R%d at line %d\n", reg_dest_addr, reg_src_addr, cpu->ip + 1);
        return;
    }

    int dest_addr = cpu->registers[reg_dest_addr];
    int src_addr = cpu->registers[reg_src_addr];

    if (!isValidMem(dest_addr) || !isValidMem(src_addr)) {
        fprintf(stderr, "Error STRCPY: Invalid memory address 0x%X or 0x%X at line %d\n", dest_addr, src_addr, cpu->ip + 1);
        return;
    }

    if (dest_addr == src_addr) {
        return;
    }

    int offset = 0;
    int character;
    while (true) {
        int current_src_addr = src_addr + offset;
        int current_dest_addr = dest_addr + offset;

        if (!isValidMem(current_src_addr) || !isValidMem(current_dest_addr)) {
            fprintf(stderr, "Error STRCPY: Memory access out of bounds during copy at line %d\n", cpu->ip + 1);
            if (isValidMem(dest_addr + offset - 1) && offset > 0) {
                cpu->memory[dest_addr + offset - 1] = 0;
            }
            return;
        }

        character = cpu->memory[current_src_addr];
        cpu->memory[current_dest_addr] = character;

        if (character == 0) {
            break;
        }

        offset++;
        if (offset >= MEMORY_SIZE) {
            fprintf(stderr, "Warning STRCPY: Copy exceeded safety limit at line %d. Destination might not be null-terminated.\n", cpu->ip + 1);
            if (isValidMem(current_dest_addr)) {
                cpu->memory[current_dest_addr] = 0;
            }
            break;
        }
    }
}

void memcpy_op(VirtualCPU* cpu, int dest_addr_reg, int src_addr_reg, int len_reg) {
    if (!isValidReg(dest_addr_reg) || !isValidReg(src_addr_reg) || !isValidReg(len_reg)) {
        fprintf(stderr, "Error MEMCPY: Invalid register operand at line %d\n", cpu->ip + 1);
        return;
    }

    int dest_addr = cpu->registers[dest_addr_reg];
    int src_addr = cpu->registers[src_addr_reg];
    int len = cpu->registers[len_reg];

    if (len <= 0) {
        return;
    }

    if (!isValidMem(src_addr) || !isValidMem(src_addr + len - 1)) {
        fprintf(stderr, "Error MEMCPY: Source memory range [0x%X - 0x%X] is invalid at line %d\n",
            src_addr, src_addr + len - 1, cpu->ip + 1);
        return;
    }
    if (!isValidMem(dest_addr) || !isValidMem(dest_addr + len - 1)) {
        fprintf(stderr, "Error MEMCPY: Destination memory range [0x%X - 0x%X] is invalid at line %d\n",
            dest_addr, dest_addr + len - 1, cpu->ip + 1);
        return;
    }

    if ((src_addr < dest_addr && src_addr + len > dest_addr) ||
        (dest_addr < src_addr && dest_addr + len > src_addr)) {
        fprintf(stderr, "Warning MEMCPY: Source and destination memory regions overlap at line %d. Behavior might be undefined.\n", cpu->ip + 1);
    }

    for (int i = 0; i < len; i++) {
        if (isValidMem(src_addr + i) && isValidMem(dest_addr + i)) {
            cpu->memory[dest_addr + i] = cpu->memory[src_addr + i];
        }
        else {
            fprintf(stderr, "FATAL Error MEMCPY: Bounds exceeded *during* copy at line %d! Should not happen.\n", cpu->ip + 1);
            cpu->ip = MEMORY_SIZE + 1;
            break;
        }
    }
}

void memset_op(VirtualCPU* cpu, int dest_addr_reg, int val_reg, int len_reg) {
    if (!isValidReg(dest_addr_reg) || !isValidReg(val_reg) || !isValidReg(len_reg)) {
        fprintf(stderr, "Error MEMSET: Invalid register operand at line %d\n", cpu->ip + 1);
        return;
    }

    int dest_addr = cpu->registers[dest_addr_reg];
    int value = cpu->registers[val_reg];
    int len = cpu->registers[len_reg];
    unsigned char byte_value = value & 0xFF;

    if (len <= 0) {
        return;
    }

    if (!isValidMem(dest_addr) || !isValidMem(dest_addr + len - 1)) {
        fprintf(stderr, "Error MEMSET: Destination memory range [0x%X - 0x%X] is invalid at line %d\n",
            dest_addr, dest_addr + len - 1, cpu->ip + 1);
        return;
    }

    for (int i = 0; i < len; i++) {
        if (isValidMem(dest_addr + i)) {
            cpu->memory[dest_addr + i] = byte_value;
        }
        else {
            fprintf(stderr, "FATAL Error MEMSET: Bounds exceeded *during* set at line %d! Should not happen.\n", cpu->ip + 1);
            cpu->ip = MEMORY_SIZE + 1;
            break;
        }
    }
}

void cpuid_op(VirtualCPU* cpu, int dest_reg) {
    if (!isValidReg(dest_reg)) {
        fprintf(stderr, "Error CPUID: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
        return;
    }
    cpu->registers[dest_reg] = CPU_VERSION;
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
    VirtualCPU* cpu_ptr = NULL;
    bool sdl_initialized = false;
    bool cpu_initialized = false;

    printf("Allocating Virtual CPU on heap...\n");
    cpu_ptr = (VirtualCPU*)malloc(sizeof(VirtualCPU));
    if (!cpu_ptr) {
        fprintf(stderr, "Fatal: Failed to allocate memory for VirtualCPU struct.\n");
        return 1;
    }
    printf("CPU Struct Allocated.\n");


    printf("Initializing Virtual CPU...\n");
    if (!init_cpu(cpu_ptr)) {
        fprintf(stderr, "CPU Initialization failed.\n");
        free(cpu_ptr);
        return 1;
    }
    cpu_initialized = true;
    printf("CPU Initialized.\n");

    if (!initialize_disk_image(cpu_ptr)) {
        fprintf(stderr, "Failed to initialize disk image. Continuing without disk support...\n");
    }

    printf("Initializing SDL...\n");
    if (!init_sdl(cpu_ptr)) {
        fprintf(stderr, "SDL Initialization failed.\n");
        if (cpu_initialized) cleanup_cpu(cpu_ptr);
        free(cpu_ptr);
        return 1;
    }
    sdl_initialized = true;
    printf("SDL Initialized.\n");

#ifdef _WIN32
    enable_virtual_terminal_processing_if_needed();
    printf("Virtual terminal processing checked/enabled (Windows).\n");
#endif

    printf("Loading program '%s'...\n");
    program_size = loadProgram(filename, program, MAX_PROGRAM_SIZE);

    if (program_size < 0) {
        fprintf(stderr, "Failed to load program (Error code: %d).\n", program_size);
        if (sdl_initialized) cleanup_sdl(cpu_ptr);
        if (cpu_initialized) cleanup_cpu(cpu_ptr);
        free(cpu_ptr);
        return 1;
    }
    printf("Program loaded successfully (%d lines).\n", program_size);


    printf("Starting execution...\n");
    execute(cpu_ptr, program, program_size);

    printf("Cleaning up...\n");
    for (int i = 0; i < program_size; i++) {
        free(program[i]);
        program[i] = NULL;
    }
    printf("Program memory freed.\n");

    if (sdl_initialized) {
        cleanup_sdl(cpu_ptr);
        printf("SDL resources cleaned up.\n");
    }

    if (cpu_initialized) {
        cleanup_cpu(cpu_ptr);
        printf("CPU resources cleaned up.\n");
    }

    free(cpu_ptr);
    printf("CPU struct memory freed.\n");

    printf("Exiting.\n");
    return 0;
}
