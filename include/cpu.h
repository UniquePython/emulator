#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE 65536

typedef uint16_t u16;

typedef struct cpu
{
    u16 memory[MEMORY_SIZE];
    u16 reg[16];
    u16 pc;
    bool running;
} CPU;

void cpu_init(CPU *cpu);
void cpu_load(CPU *cpu, const char *filename);
void cpu_run(CPU *cpu);

#endif