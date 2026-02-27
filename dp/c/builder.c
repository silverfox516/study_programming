#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Computer {
    char* cpu;
    char* ram;
    char* storage;
    char* gpu;
    char* motherboard;
    int has_wifi;
    int has_bluetooth;
} Computer;

typedef struct ComputerBuilder {
    Computer* computer;
    struct ComputerBuilder* (*set_cpu)(struct ComputerBuilder* self, const char* cpu);
    struct ComputerBuilder* (*set_ram)(struct ComputerBuilder* self, const char* ram);
    struct ComputerBuilder* (*set_storage)(struct ComputerBuilder* self, const char* storage);
    struct ComputerBuilder* (*set_gpu)(struct ComputerBuilder* self, const char* gpu);
    struct ComputerBuilder* (*set_motherboard)(struct ComputerBuilder* self, const char* motherboard);
    struct ComputerBuilder* (*add_wifi)(struct ComputerBuilder* self);
    struct ComputerBuilder* (*add_bluetooth)(struct ComputerBuilder* self);
    Computer* (*build)(struct ComputerBuilder* self);
    void (*destroy)(struct ComputerBuilder* self);
} ComputerBuilder;

ComputerBuilder* builder_set_cpu(ComputerBuilder* self, const char* cpu) {
    if (self->computer->cpu) free(self->computer->cpu);
    self->computer->cpu = malloc(strlen(cpu) + 1);
    strcpy(self->computer->cpu, cpu);
    return self;
}

ComputerBuilder* builder_set_ram(ComputerBuilder* self, const char* ram) {
    if (self->computer->ram) free(self->computer->ram);
    self->computer->ram = malloc(strlen(ram) + 1);
    strcpy(self->computer->ram, ram);
    return self;
}

ComputerBuilder* builder_set_storage(ComputerBuilder* self, const char* storage) {
    if (self->computer->storage) free(self->computer->storage);
    self->computer->storage = malloc(strlen(storage) + 1);
    strcpy(self->computer->storage, storage);
    return self;
}

ComputerBuilder* builder_set_gpu(ComputerBuilder* self, const char* gpu) {
    if (self->computer->gpu) free(self->computer->gpu);
    self->computer->gpu = malloc(strlen(gpu) + 1);
    strcpy(self->computer->gpu, gpu);
    return self;
}

ComputerBuilder* builder_set_motherboard(ComputerBuilder* self, const char* motherboard) {
    if (self->computer->motherboard) free(self->computer->motherboard);
    self->computer->motherboard = malloc(strlen(motherboard) + 1);
    strcpy(self->computer->motherboard, motherboard);
    return self;
}

ComputerBuilder* builder_add_wifi(ComputerBuilder* self) {
    self->computer->has_wifi = 1;
    return self;
}

ComputerBuilder* builder_add_bluetooth(ComputerBuilder* self) {
    self->computer->has_bluetooth = 1;
    return self;
}

Computer* builder_build(ComputerBuilder* self) {
    Computer* result = self->computer;
    self->computer = NULL;  // Transfer ownership
    return result;
}

void builder_destroy(ComputerBuilder* self) {
    if (self->computer) {
        if (self->computer->cpu) free(self->computer->cpu);
        if (self->computer->ram) free(self->computer->ram);
        if (self->computer->storage) free(self->computer->storage);
        if (self->computer->gpu) free(self->computer->gpu);
        if (self->computer->motherboard) free(self->computer->motherboard);
        free(self->computer);
    }
    free(self);
}

ComputerBuilder* create_computer_builder() {
    ComputerBuilder* builder = malloc(sizeof(ComputerBuilder));
    builder->computer = malloc(sizeof(Computer));

    // Initialize computer
    builder->computer->cpu = NULL;
    builder->computer->ram = NULL;
    builder->computer->storage = NULL;
    builder->computer->gpu = NULL;
    builder->computer->motherboard = NULL;
    builder->computer->has_wifi = 0;
    builder->computer->has_bluetooth = 0;

    // Set function pointers
    builder->set_cpu = builder_set_cpu;
    builder->set_ram = builder_set_ram;
    builder->set_storage = builder_set_storage;
    builder->set_gpu = builder_set_gpu;
    builder->set_motherboard = builder_set_motherboard;
    builder->add_wifi = builder_add_wifi;
    builder->add_bluetooth = builder_add_bluetooth;
    builder->build = builder_build;
    builder->destroy = builder_destroy;

    return builder;
}

void print_computer(Computer* computer) {
    printf("Computer Configuration:\n");
    printf("  CPU: %s\n", computer->cpu ? computer->cpu : "Not specified");
    printf("  RAM: %s\n", computer->ram ? computer->ram : "Not specified");
    printf("  Storage: %s\n", computer->storage ? computer->storage : "Not specified");
    printf("  GPU: %s\n", computer->gpu ? computer->gpu : "Not specified");
    printf("  Motherboard: %s\n", computer->motherboard ? computer->motherboard : "Not specified");
    printf("  WiFi: %s\n", computer->has_wifi ? "Yes" : "No");
    printf("  Bluetooth: %s\n", computer->has_bluetooth ? "Yes" : "No");
}

void destroy_computer(Computer* computer) {
    if (computer) {
        if (computer->cpu) free(computer->cpu);
        if (computer->ram) free(computer->ram);
        if (computer->storage) free(computer->storage);
        if (computer->gpu) free(computer->gpu);
        if (computer->motherboard) free(computer->motherboard);
        free(computer);
    }
}

int main() {
    printf("--- Computer Builder Pattern ---\n\n");

    // Gaming Computer
    printf("Building Gaming Computer:\n");
    ComputerBuilder* gaming_builder = create_computer_builder();
    Computer* gaming_pc = gaming_builder
        ->set_cpu(gaming_builder, "Intel i9-13900K")
        ->set_ram(gaming_builder, "32GB DDR5")
        ->set_storage(gaming_builder, "1TB NVMe SSD")
        ->set_gpu(gaming_builder, "RTX 4080")
        ->set_motherboard(gaming_builder, "ASUS ROG Strix Z790")
        ->add_wifi(gaming_builder)
        ->add_bluetooth(gaming_builder)
        ->build(gaming_builder);

    print_computer(gaming_pc);
    gaming_builder->destroy(gaming_builder);

    printf("\n" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n\n");

    // Office Computer
    printf("Building Office Computer:\n");
    ComputerBuilder* office_builder = create_computer_builder();
    Computer* office_pc = office_builder
        ->set_cpu(office_builder, "Intel i5-13400")
        ->set_ram(office_builder, "16GB DDR4")
        ->set_storage(office_builder, "512GB SSD")
        ->set_motherboard(office_builder, "MSI Pro B660M")
        ->add_wifi(office_builder)
        ->build(office_builder);

    print_computer(office_pc);
    office_builder->destroy(office_builder);

    printf("\n" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n\n");

    // Budget Computer
    printf("Building Budget Computer:\n");
    ComputerBuilder* budget_builder = create_computer_builder();
    Computer* budget_pc = budget_builder
        ->set_cpu(budget_builder, "AMD Ryzen 5 5600G")
        ->set_ram(budget_builder, "8GB DDR4")
        ->set_storage(budget_builder, "256GB SSD")
        ->build(budget_builder);

    print_computer(budget_pc);
    budget_builder->destroy(budget_builder);

    // Cleanup
    destroy_computer(gaming_pc);
    destroy_computer(office_pc);
    destroy_computer(budget_pc);

    return 0;
}