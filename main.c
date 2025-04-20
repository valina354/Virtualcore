#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include "Windows.h"
#endif
#include <time.h>
#include <SDL.h>
#include <stdbool.h>

#define MEMORY_SIZE (128 * 1024)
#define NUM_REGISTERS 16
#define MAX_PROGRAM_SIZE 1024

typedef enum { MOV, ADD, SUB, MUL, DIV, INTR, NOP, HLT, NOT, AND, OR, XOR, SHL, SHR, JMP, CMP, JNE, JMPH, JMPL, NEG, INC ,DEC, XCHG, CLR, PUSH, POP, CALL, RET, ROL, ROR, STRMOV, RND, JEQ, MOD,POW,SQRT,ABS} InstructionType;

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
    SDL_Color palette[16];
    int screen_on;
} VirtualCPU;

InstructionType parseInstruction(char* instruction) {
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
    return -1;
}

void mov(VirtualCPU* cpu, int reg1, int value) {
    cpu->registers[reg1] = value;
}

void add(VirtualCPU* cpu, int reg1, int reg2) {
    cpu->registers[reg1] += cpu->registers[reg2];
}

void sub(VirtualCPU* cpu, int reg1, int reg2) {
    cpu->registers[reg1] -= cpu->registers[reg2];
}

void mul(VirtualCPU* cpu, int reg1, int reg2) {
    cpu->registers[reg1] *= cpu->registers[reg2];
}

void divi(VirtualCPU* cpu, int reg1, int reg2) {
    if (cpu->registers[reg2] != 0) {
        cpu->registers[reg1] /= cpu->registers[reg2];
    }
    else {
        printf("Error: Division by zero.\n");
    }
}

void audioCallback(void* userdata, Uint8* stream, int len) {
    VirtualCPU* cpu = (VirtualCPU*)userdata;

    Sint16* audio_stream = (Sint16*)stream;

    static double time = 0.0;
    double increment = cpu->frequency / cpu->audioSpec.freq;

    for (int i = 0; i < len / 2; i++) {
        if (cpu->speaker_on) {
            if (cpu->sleep_duration > 0) {
                Uint32 current_time = SDL_GetTicks();
                if (current_time - cpu->sleep_start_time < (Uint32)cpu->sleep_duration) {
                    audio_stream[i] = 0;
                }
                else {
                    cpu->sleep_duration = 0;
                    cpu->sleep_start_time = 0;
                    if (sin(2.0 * M_PI * time) > 0) {
                        audio_stream[i] = (Sint16)(32767 * cpu->volume);
                    }
                    else {
                        audio_stream[i] = (Sint16)(-32767 * cpu->volume);
                    }
                }
            }
            else {
                if (sin(2.0 * M_PI * time) > 0) {
                    audio_stream[i] = (Sint16)(32767 * cpu->volume);
                }
                else {
                    audio_stream[i] = (Sint16)(-32767 * cpu->volume);
                }
            }
        }
        else {
            audio_stream[i] = 0;
        }
        time += increment;
        if (time > 1.0) time -= 1.0;
    }
}

void updateScreen(VirtualCPU* cpu) {
    if (cpu->screen_on) {
        SDL_UpdateTexture(cpu->texture, NULL, cpu->pixels, cpu->screen_width * sizeof(Uint32));
        SDL_RenderClear(cpu->renderer);
        SDL_RenderCopy(cpu->renderer, cpu->texture, NULL, NULL);
        SDL_RenderPresent(cpu->renderer);
    }
}

// Interrupt handler
// 0x01: Print integer in R0
// 0x02: Print string from memory address in R0 until null terminator
// 0x03: Prints a null terminator
// 0x04: Sleep for milliseconds specified in R0
// 0x05: Keyboard input, reads a character into R0
// 0x06: Full CPU reset
void interrupt(VirtualCPU* cpu, int interrupt_id) {
    if (interrupt_id == 0x01) {
        printf("%d\n", cpu->registers[0]);
    }
    else if (interrupt_id == 0x02) {
        int addr = cpu->registers[0];
        while (cpu->memory[addr] != 0) {
            putchar(cpu->memory[addr]);
            addr++;
        }
        putchar('\n');
    }
    else if (interrupt_id == 0x03) {
        putchar('\n');
    }
    else if (interrupt_id == 0x04) {
        int sleep_ms = cpu->registers[0];
        if (sleep_ms > 0) {
#ifdef _WIN32
            Sleep(sleep_ms);
#else
            if (usleep(sleep_ms * 1000) != 0) {
                perror("usleep");
                printf("Error: Sleep interrupt failed.\n");
            }
#endif
        }
        else {
            printf("Warning: Sleep duration is zero or negative: %d ms. Ignoring sleep interrupt.\n", sleep_ms);
        }
    }
    else if (interrupt_id == 0x05) {
        cpu->registers[0] = getchar();
    }
    else if (interrupt_id == 0x06) {
        for (int i = 0; i < NUM_REGISTERS; i++) {
            cpu->registers[i] = 0;
        }
        for (int i = 0; i < MEMORY_SIZE; i++) {
            cpu->memory[i] = 0;
        }
        cpu->ip = 0;
        cpu->flags = 0;
    }
    else if (interrupt_id == 0x07) {
        time_t timer;
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);

        int hours = tm_info->tm_hour;
        int minutes = tm_info->tm_min;
        int seconds = tm_info->tm_sec;

        cpu->registers[0] = hours;
        cpu->registers[1] = minutes;
        cpu->registers[2] = seconds;
    }
    else if (interrupt_id == 0x08) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    else if (interrupt_id == 0x09) {
        cpu->speaker_on = 1;
    }
    else if (interrupt_id == 0x10) {
        cpu->speaker_on = 0;
        cpu->sleep_duration = 0;
        cpu->sleep_start_time = 0;
    }
    else if (interrupt_id == 0x11) {
        cpu->frequency = (double)cpu->registers[0];
        if (cpu->frequency <= 0) cpu->frequency = 1.0;
    }
    else if (interrupt_id == 0x12) {
        cpu->volume = (double)cpu->registers[0] / 100.0;
        if (cpu->volume < 0.0) cpu->volume = 0.0;
        if (cpu->volume > 1.0) cpu->volume = 1.0;
    }
    else if (interrupt_id == 0x13) {
        cpu->sleep_duration = cpu->registers[0];
        cpu->sleep_start_time = SDL_GetTicks();
    }
    else if (interrupt_id == 0x14) {
        int x = cpu->registers[0];
        int y = cpu->registers[1];
        int color_index = cpu->registers[2];

        if (x >= 0 && x < cpu->screen_width && y >= 0 && y < cpu->screen_height && color_index >= 0 && color_index < 16) {
            Uint32 color = 0;
            SDL_Color palette_color = cpu->palette[color_index];
            color = (palette_color.a << 24) | (palette_color.r << 16) | (palette_color.g << 8) | palette_color.b;
            cpu->pixels[y * cpu->screen_width + x] = color;
        }
    }
    else if (interrupt_id == 0x15) {
        int color_index = cpu->registers[0];
        if (color_index >= 0 && color_index < 16) {
            Uint32 clear_color = 0;
            SDL_Color palette_color = cpu->palette[color_index];
            clear_color = (palette_color.a << 24) | (palette_color.r << 16) | (palette_color.g << 8) | palette_color.b;
            for (int i = 0; i < cpu->screen_width * cpu->screen_height; i++) {
                cpu->pixels[i] = clear_color;
            }
        }
    }
    else if (interrupt_id == 0x16) {
        cpu->screen_on = 1;
    }
    else if (interrupt_id == 0x17) {
        cpu->screen_on = 0;
    }
    else if (interrupt_id == 0x18) {
        int new_width = cpu->registers[0];
        int new_height = cpu->registers[1];

        if (new_width > 0 && new_height > 0 && new_width <= 1024 && new_height <= 1024) {
            cpu->screen_width = new_width;
            cpu->screen_height = new_height;

            SDL_DestroyTexture(cpu->texture);
            cpu->texture = SDL_CreateTexture(cpu->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, cpu->screen_width, cpu->screen_height);
            if (!cpu->texture) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error (resolution change): %s", SDL_GetError());
            }

            free(cpu->pixels);
            cpu->pixels = (Uint32*)malloc(cpu->screen_width * cpu->screen_height * sizeof(Uint32));
            if (!cpu->pixels) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to reallocate pixel buffer (resolution change)");
            }
            else {
                memset(cpu->pixels, 0, cpu->screen_width * cpu->screen_height * sizeof(Uint32));
            }

            SDL_SetWindowSize(cpu->window, cpu->screen_width, cpu->screen_height);
        }
        else {
            printf("Warning: Invalid screen resolution requested: %dx%d\n", new_width, new_height);
        }
    }
    else if (interrupt_id == 0x19) {
        int buffer_addr = cpu->registers[0];
        int max_length = cpu->registers[1];
        char input_buffer[256];
        if (max_length > 255) max_length = 255;

        fgets(input_buffer, max_length + 1, stdin);

        size_t len = strlen(input_buffer);
        if (len > 0 && input_buffer[len - 1] == '\n') {
            input_buffer[len - 1] = '\0';
        }

        for (int i = 0; input_buffer[i] != '\0'; i++) {
            cpu->memory[buffer_addr + i] = input_buffer[i];
        }
        cpu->memory[buffer_addr + strlen(input_buffer)] = 0;
    }
    else if (interrupt_id == 0x20) {
        printf("%x\n", cpu->registers[0]);
    }
    else if (interrupt_id == 0x21) {
        printf("Registers:\n");
        for (int i = 0; i < NUM_REGISTERS; i++) {
            printf("R%d: %d (0x%x)\n", i, cpu->registers[i], cpu->registers[i]);
        }
    }
    else if (interrupt_id == 0x22) {
        int addr = cpu->registers[0];
        int length = cpu->registers[1];
        printf("Memory Dump from 0x%x, length %d:\n", addr, length);
        for (int i = 0; i < length; i++) {
            if ((i % 16) == 0) {
                printf("\n0x%04x: ", addr + i);
            }
            printf("%02x ", cpu->memory[addr + i] & 0xFF);
        }
        printf("\n");
    }
    else if (interrupt_id == 0x23) {
        cpu->registers[0] = SDL_GetTicks();
    }
    else {
        printf("Error: Unknown interrupt 0x%x\n", interrupt_id);
    }
}

void nop(VirtualCPU* cpu) {
}

void not(VirtualCPU* cpu, int reg1) {
    cpu->registers[reg1] = ~cpu->registers[reg1];
}

void and(VirtualCPU* cpu, int reg1, int reg2) {
    cpu->registers[reg1] &= cpu->registers[reg2];
}

void or(VirtualCPU* cpu, int reg1, int reg2) {
    cpu->registers[reg1] |= cpu->registers[reg2];
}

void xor(VirtualCPU* cpu, int reg1, int reg2) {
    cpu->registers[reg1] ^= cpu->registers[reg2];
}

void shl(VirtualCPU* cpu, int reg1, int count) {
    cpu->registers[reg1] <<= count;
}

void shr(VirtualCPU* cpu, int reg1, int count) {
    cpu->registers[reg1] >>= count;
}
void jmp(VirtualCPU* cpu, int line_number) {
    if (line_number >= 1) {
        cpu->ip = line_number;
        if (cpu->ip < 0) cpu->ip = 0;
    }
    else {
        printf("Error: Invalid jump line number.\n");
    }
}

void cmp(VirtualCPU* cpu, int reg1, int reg2) {
    int result = cpu->registers[reg1] - cpu->registers[reg2];

    if (result == 0) {
        cpu->flags |= 0x01;
    }
    else {
        cpu->flags &= ~0x01;
    }

    if (result > 0) {
        cpu->flags |= 0x02;
        cpu->flags &= ~0x04; 
    }
    else if (result < 0) {
        cpu->flags &= ~0x02; 
        cpu->flags |= 0x04; 
    }
    else {
        cpu->flags &= ~0x02; 
        cpu->flags &= ~0x04;
    }
}

void neg(VirtualCPU* cpu, int reg1) {
    cpu->registers[reg1] = -cpu->registers[reg1];
}

void inc(VirtualCPU* cpu, int reg1) {
    cpu->registers[reg1]++;
}

void dec(VirtualCPU* cpu, int reg1) {
    cpu->registers[reg1]--;
}

void xchg(VirtualCPU* cpu, int reg1, int reg2) {
    int temp = cpu->registers[reg1];
    cpu->registers[reg1] = cpu->registers[reg2];
    cpu->registers[reg2] = temp;
}

void clr(VirtualCPU* cpu, int reg1) {
    cpu->registers[reg1] = 0;
}

void push(VirtualCPU* cpu, int reg1) {
    cpu->registers[15]--;
    if (cpu->registers[15] < 0) {
        printf("Error: Stack Overflow\n");
        cpu->registers[15] = 0;
        return;
    }
    cpu->memory[cpu->registers[15]] = cpu->registers[reg1];
}

void pop(VirtualCPU* cpu, int reg1) {
    if (cpu->registers[15] >= MEMORY_SIZE) {
        printf("Error: Stack Underflow\n");
        cpu->registers[15] = MEMORY_SIZE - 1;
        return;
    }
    cpu->registers[reg1] = cpu->memory[cpu->registers[15]];
    cpu->registers[15]++;
}

void call(VirtualCPU* cpu, int line_number) {
    cpu->registers[15]--;
    if (cpu->registers[15] < 0) {
        printf("Error: Stack Overflow\n");
        cpu->registers[15] = 0;
        return;
    }
    cpu->memory[cpu->registers[15]] = cpu->ip + 1;
    jmp(cpu, line_number);
}

void ret(VirtualCPU* cpu) {
    if (cpu->registers[15] >= MEMORY_SIZE) {
        printf("Error: Stack Underflow during RET\n");
        cpu->registers[15] = MEMORY_SIZE - 1;
        return;
    }
    cpu->ip = cpu->memory[cpu->registers[15]];
    cpu->registers[15]++;
}

void rol(VirtualCPU* cpu, int reg1, int count) {
    unsigned int val = cpu->registers[reg1];
    int bit_size = sizeof(cpu->registers[reg1]) * 8;
    count %= bit_size;
    cpu->registers[reg1] = (val << count) | (val >> (bit_size - count));
}

void ror(VirtualCPU* cpu, int reg1, int count) {
    unsigned int val = cpu->registers[reg1];
    int bit_size = sizeof(cpu->registers[reg1]) * 8;
    count %= bit_size;
    cpu->registers[reg1] = (val >> count) | (val << (bit_size - count));
}

void rnd(VirtualCPU* cpu, int reg1) {
    cpu->registers[reg1] = rand();
}

void mod_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (cpu->registers[reg2] != 0) {
        cpu->registers[reg1] %= cpu->registers[reg2];
    }
    else {
        printf("Error: Modulo by zero.\n");
    }
}

void pow_op(VirtualCPU* cpu, int reg1, int reg2) {
    int base = cpu->registers[reg1];
    int exponent = cpu->registers[reg2];
    int result = 1;
    if (exponent >= 0) {
        for (int i = 0; i < exponent; i++) {
            result *= base;
        }
    }
    else {
        result = 0;
        printf("Warning: Negative exponent in POW, result set to 0.\n");
    }
    cpu->registers[reg1] = result;
}

void sqrt_op(VirtualCPU* cpu, int reg1) {
    if (cpu->registers[reg1] >= 0) {
        cpu->registers[reg1] = sqrt(cpu->registers[reg1]);
    }
    else {
        cpu->registers[reg1] = 0;
        printf("Warning: Negative input to SQRT, result set to 0.\n");
    }
}

void abs_op(VirtualCPU* cpu, int reg1) {
    if (cpu->registers[reg1] < 0) {
        cpu->registers[reg1] = -cpu->registers[reg1];
    }
}

void execute(VirtualCPU* cpu, char* program[], int program_size) {
    srand(time(NULL));
    while (cpu->ip < program_size) {
        char* instruction = program[cpu->ip];
        char op[5];
        sscanf(instruction, "%s", op);
        InstructionType inst = parseInstruction(op);

        if (inst == -1) {
            printf("Error: Unknown instruction at line %d: %s\n", cpu->ip + 1, instruction);
            break;
        }

        if (inst == MOV) {
            int reg, value;
            sscanf(instruction, "MOV R%d, %d", &reg, &value);
            mov(cpu, reg, value);
        }
        else if (inst == ADD) {
            int reg1, reg2;
            sscanf(instruction, "ADD R%d, R%d", &reg1, &reg2);
            add(cpu, reg1, reg2);
        }
        else if (inst == SUB) {
            int reg1, reg2;
            sscanf(instruction, "SUB R%d, R%d", &reg1, &reg2);
            sub(cpu, reg1, reg2);
        }
        else if (inst == MUL) {
            int reg1, reg2;
            sscanf(instruction, "MUL R%d, R%d", &reg1, &reg2);
            mul(cpu, reg1, reg2);
        }
        else if (inst == DIV) {
            int reg1, reg2;
            sscanf(instruction, "DIV R%d, R%d", &reg1, &reg2);
            divi(cpu, reg1, reg2);
        }
        else if (inst == INTR) {
            int interrupt_id;
            sscanf(instruction, "INT %x", &interrupt_id);
            interrupt(cpu, interrupt_id);
        }
        else if (inst == NOP) {
            nop(cpu);
        }
        else if (inst == HLT) {
            while (true) {
            }
        }
        else if (inst == NOT) {
            int reg1;
            sscanf(instruction, "NOT R%d", &reg1);
            not(cpu, reg1);
        }
        else if (inst == AND) {
            int reg1, reg2;
            sscanf(instruction, "AND R%d, R%d", &reg1, &reg2);
            and (cpu, reg1, reg2);
        }
        else if (inst == OR) {
            int reg1, reg2;
            sscanf(instruction, "OR R%d, R%d", &reg1, &reg2);
            or (cpu, reg1, reg2);
        }
        else if (inst == XOR) {
            int reg1, reg2;
            sscanf(instruction, "XOR R%d, R%d", &reg1, &reg2);
            xor (cpu, reg1, reg2);
        }
        else if (inst == SHL) {
            int reg1, count;
            sscanf(instruction, "SHL R%d, %d", &reg1, &count);
            shl(cpu, reg1, count);
        }
        else if (inst == SHR) {
            int reg1, count;
            sscanf(instruction, "SHR R%d, %d", &reg1, &count);
            shr(cpu, reg1, count);
        }
        else if (inst == JMP) { 
            int line_number;
            sscanf(instruction, "JMP %d", &line_number);
            jmp(cpu, line_number);
            continue;
        }
        else if (inst == CMP) {
            int reg1, reg2;
            sscanf(instruction, "CMP R%d, R%d", &reg1, &reg2);
            cmp(cpu, reg1, reg2);
        }
        else if (inst == JNE) {
            int line_number;
            sscanf(instruction, "JNE %d", &line_number);
            if ((cpu->flags & 0x01) == 0) {
                jmp(cpu, line_number);
                continue;
            }
        }
        else if (inst == JMPH) {
            int line_number;
            sscanf(instruction, "JMPH %d", &line_number);
            if ((cpu->flags & 0x02) != 0) {
                jmp(cpu, line_number);
                continue;
            }
        }
        else if (inst == JMPL) {
            int line_number;
            sscanf(instruction, "JMPL %d", &line_number);
            if ((cpu->flags & 0x04) != 0) {
                jmp(cpu, line_number);
                continue;
            }
        }
        else if (inst == NEG) {
            int reg1;
            sscanf(instruction, "NEG R%d", &reg1);
            neg(cpu, reg1);
        }
        else if (inst == INC) {
            int reg1;
            sscanf(instruction, "INC R%d", &reg1);
            inc(cpu, reg1);
        }
        else if (inst == DEC) {
            int reg1;
            sscanf(instruction, "DEC R%d", &reg1);
            dec(cpu, reg1);
        }
        else if (inst == XCHG) {
            int reg1, reg2;
            sscanf(instruction, "XCHG R%d, R%d", &reg1, &reg2);
            xchg(cpu, reg1, reg2);
        }
        else if (inst == CLR) {
            int reg1;
            sscanf(instruction, "CLR R%d", &reg1);
            clr(cpu, reg1);
        } else if (inst == PUSH) {
            int reg1;
            sscanf(instruction, "PUSH R%d", &reg1);
            push(cpu, reg1);
        }
        else if (inst == POP) {
            int reg1;
            sscanf(instruction, "POP R%d", &reg1);
            pop(cpu, reg1);
        }
        else if (inst == CALL) {
            int line_number;
            sscanf(instruction, "CALL %d", &line_number);
            call(cpu, line_number);
            continue;
        }
        else if (inst == RET) {
            ret(cpu);
            continue;
        }
        else if (inst == ROL) {
            int reg1, count;
            sscanf(instruction, "ROL R%d, %d", &reg1, &count);
            rol(cpu, reg1, count);
        }
        else if (inst == ROR) {
            int reg1, count;
            sscanf(instruction, "ROR R%d, %d", &reg1, &count);
            ror(cpu, reg1, count);
        }
        else if (inst == STRMOV) {
            int addr;
            char str[256];
            sscanf(instruction, "STRMOV %d, \"%[^\"]\"", &addr, str);
            for (int i = 0; str[i] != '\0'; i++) {
                cpu->memory[addr + i] = str[i];
            }
            cpu->memory[addr + strlen(str)] = 0;
        }
        else if (inst == RND) {
            int reg1;
            sscanf(instruction, "RND R%d", &reg1);
            rnd(cpu, reg1);
        }
        else if (inst == JEQ) {
           int line_number;
           sscanf(instruction, "JEQ %d", &line_number);
           if ((cpu->flags & 0x01) != 0) {
               jmp(cpu, line_number);
               continue;
           }
        }
        else if (inst == MOD) {
            int reg1, reg2;
            sscanf(instruction, "MOD R%d, R%d", &reg1, &reg2);
            mod_op(cpu, reg1, reg2);
        }
        else if (inst == POW) {
            int reg1, reg2;
            sscanf(instruction, "POW R%d, R%d", &reg1, &reg2);
            pow_op(cpu, reg1, reg2);
        }
        else if (inst == SQRT) {
            int reg1;
            sscanf(instruction, "SQRT R%d", &reg1);
            sqrt_op(cpu, reg1);
        }
        else if (inst == ABS) {
            int reg1;
            sscanf(instruction, "ABS R%d", &reg1);
            abs_op(cpu, reg1);
        }
        if (cpu->screen_on) {
            updateScreen(cpu);
        }
        cpu->ip++;
    }
}

int loadProgram(const char* filename, char* program[]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    int line_count = 0;
    char buffer[256];
    char* temp_program[MAX_PROGRAM_SIZE];
    int temp_program_size = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char* comment_start = strchr(buffer, ';');
        if (comment_start != NULL) {
            *comment_start = '\0';
        }

        char* trimmed_start = buffer;
        while (isspace(*trimmed_start)) trimmed_start++;
        char* trimmed_end = trimmed_start + strlen(trimmed_start) - 1;
        while (trimmed_end > trimmed_start && isspace(*trimmed_end)) trimmed_end--;
        *(trimmed_end + 1) = '\0';

        if (strlen(trimmed_start) == 0) {
            continue;
        }

        temp_program[temp_program_size] = malloc(strlen(trimmed_start) + 1);
        strcpy(temp_program[temp_program_size], trimmed_start);
        temp_program_size++;

        if (temp_program_size >= MAX_PROGRAM_SIZE) {
            fclose(file);
            for (int i = 0; i < temp_program_size; ++i) {
                free(temp_program[i]);
            }
            return -1;
        }
    }
    fclose(file);

    typedef struct {
        char label_name[32];
        int line_number;
    } LabelEntry;
    LabelEntry label_map[MAX_PROGRAM_SIZE];
    int label_count = 0;
    int program_index = 0;
    for (int i = 0; i < temp_program_size; i++) {
        char* line = temp_program[i];
        char* label_end = strchr(line, ':');
        if (label_end != NULL) {
            *label_end = '\0';
            char* label_name = line;
            char* label_name_end = label_name + strlen(label_name) - 1;
            while (label_name_end > label_name && isspace(*label_name_end)) label_name_end--;
            *(label_name_end + 1) = '\0';

            if (strlen(label_name) > 0) {
                strcpy(label_map[label_count].label_name, label_name);
                label_map[label_count].line_number = program_index + 1;
                label_count++;
            }
            free(temp_program[i]);
            temp_program[i] = NULL;
        }
    }

    for (int i = 0; i < temp_program_size; i++) {
        if (temp_program[i] != NULL) {
            program[program_index] = temp_program[i];

            char* instruction = program[program_index];
            char op[5];
            sscanf(instruction, "%s", op);
            InstructionType inst = parseInstruction(op);

            if (inst == JMP || inst == JNE || inst == JMPH || inst == JMPL || inst == CALL) {
                char label_str[32];
                int scanned_items = sscanf(instruction, "%*s %s", label_str);

                if (scanned_items == 1) {
                    int label_line_number = -1;
                    for (int j = 0; j < label_count; j++) {
                        if (strcmp(label_map[j].label_name, label_str) == 0) {
                            label_line_number = label_map[j].line_number;
                            break;
                        }
                    }
                    if (label_line_number != -1) {
                        char new_instruction[256];
                        sprintf(new_instruction, "%s %d", op, label_line_number);
                        free(program[program_index]);
                        program[program_index] = malloc(strlen(new_instruction) + 1);
                        strcpy(program[program_index], new_instruction);
                    }
                    else {
                        printf("Error: Label '%s' not found at line %d: %s\n", label_str, program_index + 1, instruction);
                    }
                }
            }
            program_index++;
        }
    }
    return program_index;
}

int main() {
    char* program[MAX_PROGRAM_SIZE];
    const char* filename = "program.asm";
    int program_size = loadProgram(filename, program);
    if (program_size == -1) {
        return 1;
    }

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec desiredSpec;
    SDL_AudioSpec obtainedSpec;

    VirtualCPU cpu = { {0}, {0}, 0, 0 };
    cpu.registers[15] = MEMORY_SIZE;

    cpu.screen_width = 256;
    cpu.screen_height = 192;
    cpu.screen_on = 0;

    cpu.window = SDL_CreateWindow("Virtual CPU Screen", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cpu.screen_width, cpu.screen_height, SDL_WINDOW_SHOWN);
    if (!cpu.window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    cpu.renderer = SDL_CreateRenderer(cpu.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!cpu.renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(cpu.window);
        SDL_Quit();
        return 1;
    }

    cpu.texture = SDL_CreateTexture(cpu.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, cpu.screen_width, cpu.screen_height);
    if (!cpu.texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error: %s", SDL_GetError());
        SDL_DestroyRenderer(cpu.renderer);
        SDL_DestroyWindow(cpu.window);
        SDL_Quit();
        return 1;
    }

    cpu.pixels = (Uint32*)malloc(cpu.screen_width * cpu.screen_height * sizeof(Uint32));
    if (!cpu.pixels) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate pixel buffer");
        SDL_DestroyTexture(cpu.texture);
        SDL_DestroyRenderer(cpu.renderer);
        SDL_DestroyWindow(cpu.window);
        SDL_Quit();
        return 1;
    }
    memset(cpu.pixels, 0, cpu.screen_width * cpu.screen_height * sizeof(Uint32));

    cpu.palette[0] = (SDL_Color){ 0, 0, 0, 255 };        // Black
    cpu.palette[1] = (SDL_Color){ 255, 255, 255, 255 };    // White
    cpu.palette[2] = (SDL_Color){ 255, 0, 0, 255 };      // Red
    cpu.palette[3] = (SDL_Color){ 0, 255, 0, 255 };      // Green
    cpu.palette[4] = (SDL_Color){ 0, 0, 255, 255 };      // Blue
    cpu.palette[5] = (SDL_Color){ 255, 255, 0, 255 };    // Yellow
    cpu.palette[6] = (SDL_Color){ 255, 0, 255, 255 };    // Magenta
    cpu.palette[7] = (SDL_Color){ 0, 255, 255, 255 };    // Cyan
    cpu.palette[8] = (SDL_Color){ 128, 128, 128, 255 };  // Gray
    cpu.palette[9] = (SDL_Color){ 192, 192, 192, 255 };  // Light Gray
    cpu.palette[10] = (SDL_Color){ 128, 0, 0, 255 };     // Dark Red
    cpu.palette[11] = (SDL_Color){ 0, 128, 0, 255 };     // Dark Green
    cpu.palette[12] = (SDL_Color){ 0, 0, 128, 255 };     // Dark Blue
    cpu.palette[13] = (SDL_Color){ 128, 128, 0, 255 };   // Dark Yellow
    cpu.palette[14] = (SDL_Color){ 128, 0, 128, 255 };   // Dark Magenta
    cpu.palette[15] = (SDL_Color){ 0, 128, 128, 255 };   // Dark Cyan

    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 512;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = &cpu;

    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);
    if (audioDevice == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenAudioDevice Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    cpu.audioSpec = obtainedSpec;
    cpu.audioDevice = audioDevice;
    cpu.frequency = 440.0;
    cpu.volume = 0.5;
    cpu.speaker_on = 0; 
    cpu.sleep_duration = 0;
    cpu.sleep_start_time = 0;

    SDL_PauseAudioDevice(cpu.audioDevice, 0);

    execute(&cpu, program, program_size);

    free(cpu.pixels);
    SDL_DestroyTexture(cpu.texture);
    SDL_DestroyRenderer(cpu.renderer);
    SDL_DestroyWindow(cpu.window);
    SDL_CloseAudioDevice(cpu.audioDevice);
    SDL_Quit();

    for (int i = 0; i < program_size; i++) {
        free(program[i]);
    }

    return 0;
}
