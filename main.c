#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 256
#define NUM_REGISTERS 16
#define MAX_PROGRAM_SIZE 1024

typedef enum { MOV, ADD, SUB, MUL, DIV, INT, NOP, HLT, NOT, AND, OR, XOR, SHL, SHR, JMP, CMP, JNE, JMPH, JMPL, NEG, INC ,DEC, SWAP, CLR } InstructionType;

typedef struct {
    int registers[NUM_REGISTERS];
    int memory[MEMORY_SIZE];
    int ip;
    int flags;
} VirtualCPU;

InstructionType parseInstruction(char* instruction) {
    if (strcmp(instruction, "MOV") == 0) return MOV;
    if (strcmp(instruction, "ADD") == 0) return ADD;
    if (strcmp(instruction, "SUB") == 0) return SUB;
    if (strcmp(instruction, "MUL") == 0) return MUL;
    if (strcmp(instruction, "DIV") == 0) return DIV;
    if (strcmp(instruction, "INT") == 0) return INT;
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
    if (strcmp(instruction, "SWAP") == 0) return SWAP;
    if (strcmp(instruction, "CLR") == 0) return CLR;
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

void interrupt(VirtualCPU* cpu, int interrupt_id) {
    if (interrupt_id == 0x01) {
        printf("%d\n", cpu->registers[0]);
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
        cpu->ip = line_number - 1;
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

void swap(VirtualCPU* cpu, int reg1, int reg2) {
    int temp = cpu->registers[reg1];
    cpu->registers[reg1] = cpu->registers[reg2];
    cpu->registers[reg2] = temp;
}

void clr(VirtualCPU* cpu, int reg1) {
    cpu->registers[reg1] = 0;
}

void execute(VirtualCPU* cpu, char* program[], int program_size) {
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
        else if (inst == INT) {
            int interrupt_id;
            sscanf(instruction, "INT %x", &interrupt_id);
            interrupt(cpu, interrupt_id);
        }
        else if (inst == NOP) {
            nop(cpu);
        }
        else if (inst == HLT) {
            break;
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
        else if (inst == SWAP) {
            int reg1, reg2;
            sscanf(instruction, "SWAP R%d, R%d", &reg1, &reg2);
            swap(cpu, reg1, reg2);
        }
        else if (inst == CLR) {
            int reg1;
            sscanf(instruction, "CLR R%d", &reg1);
            clr(cpu, reg1);
        }

        cpu->ip++;
    }
}

int loadProgram(const char* filename, char* program[]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    int line = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char* comment_start = strchr(buffer, ';');
        if (comment_start != NULL) {
            *comment_start = '\0';
        }

        if (strlen(buffer) == 0 || buffer[0] == '\n') {
            continue;
        }

        char* trimmed = buffer;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        char* end = trimmed + strlen(trimmed) - 1;
        while (end > trimmed && (*end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';

        program[line] = malloc(strlen(trimmed) + 1);
        strcpy(program[line], trimmed);
        line++;

        if (line >= MAX_PROGRAM_SIZE) {
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return line;
}

int main() {
    char* program[MAX_PROGRAM_SIZE];
    const char* filename = "program.asm";
    int program_size = loadProgram(filename, program);
    if (program_size == -1) {
        return 1;
    }

    VirtualCPU cpu = { {0}, {0}, 0, 0 };
    execute(&cpu, program, program_size);

    for (int i = 0; i < program_size; i++) {
        free(program[i]);
    }

    return 0;
}
