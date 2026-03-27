#include "cpu.h"

#include <string.h>

void cpu_init(CPU *cpu)
{
    memset(cpu->memory, 0, MEMORY_SIZE);
    memset(cpu->reg, 0, 16);

    cpu->pc = 0;
    cpu->running = true;
}