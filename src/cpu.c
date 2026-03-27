#include "cpu.h"

#include <string.h>
#include <stdio.h>

void cpu_init(CPU *cpu)
{
    memset(cpu->memory, 0, sizeof(cpu->memory));
    memset(cpu->reg, 0, sizeof(cpu->reg));

    cpu->pc = 0;
    cpu->running = true;
}

void cpu_load(CPU *cpu, const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    if (size % 2 != 0)
    {
        fprintf(stderr, "Error: malformed binary, odd byte count\n");
        fclose(fp);
        exit(1);
    }

    if (size > MEMORY_SIZE * 2)
    {
        fprintf(stderr, "Error: binary too large, max %d words\n", MEMORY_SIZE);
        fclose(fp);
        exit(1);
    }

    u8 bytes[2];
    u16 addr = 0;
    while (fread(bytes, 1, 2, fp) == 2)
        cpu->memory[addr++] = (bytes[0] << 8) | bytes[1];

    fclose(fp);
}

void cpu_run(CPU *cpu)
{
    while (cpu->running)
    {
        u16 instr = cpu->memory[cpu->pc++];

        u8 opcode = (instr >> 12) & 0xF;
        u8 ra = (instr >> 8) & 0xF;
        u8 rb = (instr >> 4) & 0xF;
        u8 rd = instr & 0xF;
        u8 imm8 = instr & 0xFF;
        u8 mode = (instr >> 11) & 0x1;
        u16 addr = instr & 0x7FF;
        i8 offset = (i8)((instr & 0xF) << 4) >> 4;

        switch (opcode)
        {
        case 0x0:
            cpu->reg[rd] = cpu->reg[ra] + cpu->reg[rb];
            break; // add
        case 0x1:
            cpu->reg[rd] = cpu->reg[ra] - cpu->reg[rb];
            break; // sub
        case 0x2:
            cpu->reg[rd] = ~(cpu->reg[ra] & cpu->reg[rb]);
            break; // nand
        case 0x3:
            cpu->reg[rd] = cpu->reg[ra] << (cpu->reg[rb] & 0xF);
            break; // shl
        case 0x4:
            cpu->reg[rd] = cpu->reg[ra] >> (cpu->reg[rb] & 0xF);
            break; // shr
        case 0x5:
            cpu->reg[rb] = cpu->memory[cpu->reg[ra] + (i16)offset];
            break; // load
        case 0x6:
            cpu->memory[cpu->reg[ra] + (i16)offset] = cpu->reg[rb];
            break; // store
        case 0x7:
            cpu->pc = mode ? addr : cpu->reg[ra];
            break; // jmp
        case 0x8:
            if ((i16)cpu->reg[ra] < (i16)cpu->reg[rb])
                cpu->pc += offset;
            break; // jlt
        case 0x9:
            if (cpu->reg[ra] == cpu->reg[rb])
                cpu->pc += offset;
            break; // jeq
        case 0xA:  // call
            cpu->reg[0xF] = cpu->pc;
            cpu->pc = mode ? addr : cpu->reg[ra];
            break;
        case 0xC:
            cpu->reg[rd] = cpu->reg[ra];
            break; // mov
        case 0xD:
            cpu->reg[rd] = (i8)imm8;
            break; // li
        case 0xE:
            cpu->running = 0;
            break; // halt
        case 0xF:
            printf("%u\n", cpu->reg[ra]);
            break; // out
        default:
            fprintf(stderr, "Unknown opcode: 0x%X at pc 0x%X\n", opcode, cpu->pc - 1);
            cpu->running = 0;
            break;
        }
    }
}