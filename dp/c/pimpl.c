// Pimpl (Opaque Pointer) Pattern in C
// Hide implementation behind a forward-declared struct

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- "header" portion: user sees only opaque pointer ---
typedef struct Engine Engine;

Engine* engine_create(int cylinders, int hp);
void engine_start(const Engine* e);
void engine_stop(const Engine* e);
const char* engine_description(const Engine* e);
void engine_destroy(Engine* e);

// --- "implementation" portion ---
struct Engine {
    int cylinders;
    int horsepower;
    char desc[64];
};

Engine* engine_create(int cylinders, int hp) {
    Engine* e = malloc(sizeof(Engine));
    e->cylinders = cylinders;
    e->horsepower = hp;
    snprintf(e->desc, 64, "%dcyl %dhp engine", cylinders, hp);
    return e;
}

void engine_start(const Engine* e) {
    printf("[Engine] Starting %dcyl (%dhp)\n", e->cylinders, e->horsepower);
}

void engine_stop(const Engine* e) {
    printf("[Engine] Stopping\n");
}

const char* engine_description(const Engine* e) { return e->desc; }

void engine_destroy(Engine* e) { free(e); }

int main(void) {
    Engine* e = engine_create(4, 150);
    printf("%s\n", engine_description(e));
    engine_start(e);
    engine_stop(e);
    engine_destroy(e);
    return 0;
}
