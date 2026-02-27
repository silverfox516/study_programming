#include <stdio.h>
#include <stdlib.h>

// Subsystem components
typedef struct CPU {
    int temperature;
} CPU;

typedef struct Memory {
    int used_memory;
    int total_memory;
} Memory;

typedef struct HardDrive {
    int used_space;
    int total_space;
} HardDrive;

typedef struct GPU {
    int gpu_load;
} GPU;

CPU* create_cpu() {
    CPU* cpu = malloc(sizeof(CPU));
    cpu->temperature = 35;
    return cpu;
}

void cpu_start(CPU* cpu) {
    printf("CPU: Starting processor\n");
    cpu->temperature = 45;
}

void cpu_shutdown(CPU* cpu) {
    printf("CPU: Shutting down processor\n");
    cpu->temperature = 25;
}

void cpu_status(CPU* cpu) {
    printf("CPU: Temperature %d°C\n", cpu->temperature);
}

Memory* create_memory() {
    Memory* mem = malloc(sizeof(Memory));
    mem->total_memory = 16000;
    mem->used_memory = 2000;
    return mem;
}

void memory_load(Memory* mem) {
    printf("Memory: Loading operating system\n");
    mem->used_memory = 4000;
}

void memory_free(Memory* mem) {
    printf("Memory: Freeing memory\n");
    mem->used_memory = 1000;
}

void memory_status(Memory* mem) {
    printf("Memory: %d/%d MB used\n", mem->used_memory, mem->total_memory);
}

HardDrive* create_harddrive() {
    HardDrive* hd = malloc(sizeof(HardDrive));
    hd->total_space = 1000;
    hd->used_space = 250;
    return hd;
}

void harddrive_read(HardDrive* hd) {
    printf("HardDrive: Reading boot sector\n");
}

void harddrive_write(HardDrive* hd) {
    printf("HardDrive: Writing system logs\n");
}

void harddrive_status(HardDrive* hd) {
    printf("HardDrive: %d/%d GB used\n", hd->used_space, hd->total_space);
}

GPU* create_gpu() {
    GPU* gpu = malloc(sizeof(GPU));
    gpu->gpu_load = 0;
    return gpu;
}

void gpu_start(GPU* gpu) {
    printf("GPU: Initializing graphics\n");
    gpu->gpu_load = 25;
}

void gpu_shutdown(GPU* gpu) {
    printf("GPU: Shutting down graphics\n");
    gpu->gpu_load = 0;
}

void gpu_status(GPU* gpu) {
    printf("GPU: Load %d%%\n", gpu->gpu_load);
}

// Facade
typedef struct ComputerFacade {
    CPU* cpu;
    Memory* memory;
    HardDrive* harddrive;
    GPU* gpu;
} ComputerFacade;

ComputerFacade* create_computer_facade() {
    ComputerFacade* computer = malloc(sizeof(ComputerFacade));
    computer->cpu = create_cpu();
    computer->memory = create_memory();
    computer->harddrive = create_harddrive();
    computer->gpu = create_gpu();
    return computer;
}

void computer_start(ComputerFacade* computer) {
    printf("=== Starting Computer ===\n");
    cpu_start(computer->cpu);
    memory_load(computer->memory);
    harddrive_read(computer->harddrive);
    gpu_start(computer->gpu);
    printf("Computer started successfully!\n\n");
}

void computer_shutdown(ComputerFacade* computer) {
    printf("=== Shutting Down Computer ===\n");
    harddrive_write(computer->harddrive);
    memory_free(computer->memory);
    gpu_shutdown(computer->gpu);
    cpu_shutdown(computer->cpu);
    printf("Computer shut down successfully!\n\n");
}

void computer_status(ComputerFacade* computer) {
    printf("=== Computer Status ===\n");
    cpu_status(computer->cpu);
    memory_status(computer->memory);
    harddrive_status(computer->harddrive);
    gpu_status(computer->gpu);
    printf("\n");
}

void destroy_computer_facade(ComputerFacade* computer) {
    if (computer) {
        free(computer->cpu);
        free(computer->memory);
        free(computer->harddrive);
        free(computer->gpu);
        free(computer);
    }
}

int main() {
    printf("--- Computer Facade Pattern ---\n\n");

    ComputerFacade* computer = create_computer_facade();

    // Simple interface for complex operations
    computer_start(computer);
    computer_status(computer);
    computer_shutdown(computer);

    destroy_computer_facade(computer);
    return 0;
}