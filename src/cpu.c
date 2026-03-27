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