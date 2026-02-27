#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Flyweight interface
typedef struct {
    void (*render)(void* self, float x, float y, float size, const char* color);
    void (*destroy)(void* self);
} ParticleType;

// Concrete Flyweight - Bullet particle
typedef struct {
    ParticleType interface;
    char* sprite_data;
    char* texture_name;
} BulletParticle;

void bullet_render(void* self, float x, float y, float size, const char* color) {
    BulletParticle* bullet = (BulletParticle*)self;
    printf("Rendering bullet at (%.1f, %.1f) size %.1f color %s using texture '%s'\n",
           x, y, size, color, bullet->texture_name);
    printf("  Sprite data: %s\n", bullet->sprite_data);
}

void bullet_destroy(void* self) {
    BulletParticle* bullet = (BulletParticle*)self;
    if (bullet->sprite_data) {
        free(bullet->sprite_data);
    }
    if (bullet->texture_name) {
        free(bullet->texture_name);
    }
    free(bullet);
}

ParticleType* bullet_particle_create() {
    BulletParticle* bullet = malloc(sizeof(BulletParticle));
    bullet->interface.render = bullet_render;
    bullet->interface.destroy = bullet_destroy;

    bullet->sprite_data = malloc(strlen("bullet_sprite_data") + 1);
    strcpy(bullet->sprite_data, "bullet_sprite_data");

    bullet->texture_name = malloc(strlen("bullet.png") + 1);
    strcpy(bullet->texture_name, "bullet.png");

    return (ParticleType*)bullet;
}

// Concrete Flyweight - Missile particle
typedef struct {
    ParticleType interface;
    char* sprite_data;
    char* texture_name;
} MissileParticle;

void missile_render(void* self, float x, float y, float size, const char* color) {
    MissileParticle* missile = (MissileParticle*)self;
    printf("Rendering missile at (%.1f, %.1f) size %.1f color %s using texture '%s'\n",
           x, y, size, color, missile->texture_name);
    printf("  Sprite data: %s\n", missile->sprite_data);
}

void missile_destroy(void* self) {
    MissileParticle* missile = (MissileParticle*)self;
    if (missile->sprite_data) {
        free(missile->sprite_data);
    }
    if (missile->texture_name) {
        free(missile->texture_name);
    }
    free(missile);
}

ParticleType* missile_particle_create() {
    MissileParticle* missile = malloc(sizeof(MissileParticle));
    missile->interface.render = missile_render;
    missile->interface.destroy = missile_destroy;

    missile->sprite_data = malloc(strlen("missile_sprite_data") + 1);
    strcpy(missile->sprite_data, "missile_sprite_data");

    missile->texture_name = malloc(strlen("missile.png") + 1);
    strcpy(missile->texture_name, "missile.png");

    return (ParticleType*)missile;
}

// Flyweight Factory
typedef struct {
    ParticleType** flyweights;
    char** types;
    int count;
    int capacity;
} ParticleFactory;

ParticleFactory* particle_factory_create() {
    ParticleFactory* factory = malloc(sizeof(ParticleFactory));
    factory->flyweights = malloc(4 * sizeof(ParticleType*));
    factory->types = malloc(4 * sizeof(char*));
    factory->count = 0;
    factory->capacity = 4;
    return factory;
}

ParticleType* particle_factory_get_flyweight(ParticleFactory* factory, const char* type) {
    // Check if flyweight already exists
    for (int i = 0; i < factory->count; i++) {
        if (strcmp(factory->types[i], type) == 0) {
            printf("Reusing existing flyweight for type: %s\n", type);
            return factory->flyweights[i];
        }
    }

    // Create new flyweight
    printf("Creating new flyweight for type: %s\n", type);

    if (factory->count >= factory->capacity) {
        factory->capacity *= 2;
        factory->flyweights = realloc(factory->flyweights, factory->capacity * sizeof(ParticleType*));
        factory->types = realloc(factory->types, factory->capacity * sizeof(char*));
    }

    ParticleType* flyweight = NULL;
    if (strcmp(type, "bullet") == 0) {
        flyweight = bullet_particle_create();
    } else if (strcmp(type, "missile") == 0) {
        flyweight = missile_particle_create();
    }

    if (flyweight) {
        factory->flyweights[factory->count] = flyweight;
        factory->types[factory->count] = malloc(strlen(type) + 1);
        strcpy(factory->types[factory->count], type);
        factory->count++;
    }

    return flyweight;
}

void particle_factory_destroy(ParticleFactory* factory) {
    for (int i = 0; i < factory->count; i++) {
        factory->flyweights[i]->destroy(factory->flyweights[i]);
        free(factory->types[i]);
    }
    free(factory->flyweights);
    free(factory->types);
    free(factory);
}

int particle_factory_get_flyweight_count(ParticleFactory* factory) {
    return factory->count;
}

// Context - represents the extrinsic state
typedef struct {
    float x, y;
    float velocity_x, velocity_y;
    float size;
    char* color;
    ParticleType* type;
} Particle;

Particle* particle_create(float x, float y, float vx, float vy, float size,
                         const char* color, ParticleType* type) {
    Particle* particle = malloc(sizeof(Particle));
    particle->x = x;
    particle->y = y;
    particle->velocity_x = vx;
    particle->velocity_y = vy;
    particle->size = size;

    particle->color = malloc(strlen(color) + 1);
    strcpy(particle->color, color);

    particle->type = type;
    return particle;
}

void particle_update(Particle* particle, float delta_time) {
    particle->x += particle->velocity_x * delta_time;
    particle->y += particle->velocity_y * delta_time;
}

void particle_render(Particle* particle) {
    particle->type->render(particle->type, particle->x, particle->y,
                          particle->size, particle->color);
}

void particle_destroy(Particle* particle) {
    if (particle->color) {
        free(particle->color);
    }
    free(particle);
}

// Game World - manages many particles
typedef struct {
    Particle** particles;
    int count;
    int capacity;
    ParticleFactory* factory;
} GameWorld;

GameWorld* game_world_create() {
    GameWorld* world = malloc(sizeof(GameWorld));
    world->particles = malloc(10 * sizeof(Particle*));
    world->count = 0;
    world->capacity = 10;
    world->factory = particle_factory_create();
    return world;
}

void game_world_add_particle(GameWorld* world, const char* type, float x, float y,
                            float vx, float vy, float size, const char* color) {
    if (world->count >= world->capacity) {
        world->capacity *= 2;
        world->particles = realloc(world->particles, world->capacity * sizeof(Particle*));
    }

    ParticleType* flyweight = particle_factory_get_flyweight(world->factory, type);
    Particle* particle = particle_create(x, y, vx, vy, size, color, flyweight);
    world->particles[world->count++] = particle;
}

void game_world_update(GameWorld* world, float delta_time) {
    for (int i = 0; i < world->count; i++) {
        particle_update(world->particles[i], delta_time);
    }
}

void game_world_render(GameWorld* world) {
    printf("\nRendering %d particles:\n", world->count);
    for (int i = 0; i < world->count; i++) {
        printf("Particle %d: ", i + 1);
        particle_render(world->particles[i]);
    }
}

void game_world_destroy(GameWorld* world) {
    for (int i = 0; i < world->count; i++) {
        particle_destroy(world->particles[i]);
    }
    free(world->particles);
    particle_factory_destroy(world->factory);
    free(world);
}

// Client code
int main() {
    printf("=== Flyweight Pattern Demo - Game Particle System ===\n\n");

    GameWorld* world = game_world_create();

    printf("Creating particles:\n");

    // Add many bullets - should reuse the same bullet flyweight
    game_world_add_particle(world, "bullet", 10.0, 20.0, 5.0, 0.0, 1.0, "yellow");
    game_world_add_particle(world, "bullet", 15.0, 25.0, 5.0, 0.0, 1.0, "red");
    game_world_add_particle(world, "bullet", 20.0, 30.0, 5.0, 0.0, 1.0, "blue");

    // Add missiles - should reuse the same missile flyweight
    game_world_add_particle(world, "missile", 50.0, 60.0, 3.0, 2.0, 2.0, "white");
    game_world_add_particle(world, "missile", 55.0, 65.0, 3.0, 2.0, 2.0, "orange");

    // Add more bullets - should reuse existing bullet flyweight
    game_world_add_particle(world, "bullet", 30.0, 35.0, 5.0, 0.0, 1.0, "green");

    printf("\nTotal flyweight objects created: %d\n",
           particle_factory_get_flyweight_count(world->factory));
    printf("Total particle instances: %d\n", world->count);

    // Simulate one frame
    printf("\nSimulating game frame:");
    game_world_update(world, 0.016f); // 60 FPS
    game_world_render(world);

    // Cleanup
    game_world_destroy(world);

    printf("\n=== Memory Efficiency Demo ===\n");
    printf("Without Flyweight: Each particle would store its own sprite data\n");
    printf("  - 6 particles × ~50 bytes sprite data = ~300 bytes\n");
    printf("With Flyweight: Sprite data is shared\n");
    printf("  - 2 flyweight objects × ~50 bytes = ~100 bytes\n");
    printf("  - 6 particles × ~40 bytes extrinsic data = ~240 bytes\n");
    printf("  - Total: ~340 bytes (but scales much better with more particles)\n");

    return 0;
}