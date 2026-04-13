// Unit of Work Pattern in C

#include <stdio.h>
#include <string.h>

#define MAX_CHANGES 32

typedef struct { int id; char name[32]; int version; } Entity;
typedef enum { CHG_INSERT, CHG_UPDATE, CHG_DELETE } ChangeType;
typedef struct { ChangeType type; Entity entity; } Change;

typedef struct {
    Change changes[MAX_CHANGES];
    int count;
} UnitOfWork;

void uow_init(UnitOfWork* uow) { uow->count = 0; }

void uow_register_new(UnitOfWork* uow, Entity e) {
    printf("[UoW] NEW: id=%d name=%s\n", e.id, e.name);
    uow->changes[uow->count++] = (Change){CHG_INSERT, e};
}

void uow_register_dirty(UnitOfWork* uow, Entity e) {
    printf("[UoW] DIRTY: id=%d name=%s v%d\n", e.id, e.name, e.version);
    uow->changes[uow->count++] = (Change){CHG_UPDATE, e};
}

void uow_register_deleted(UnitOfWork* uow, int id) {
    printf("[UoW] DELETE: id=%d\n", id);
    Entity e = {.id = id};
    uow->changes[uow->count++] = (Change){CHG_DELETE, e};
}

void uow_commit(UnitOfWork* uow) {
    printf("\n[UoW] Committing %d changes...\n", uow->count);
    for (int i = 0; i < uow->count; i++) {
        Change* c = &uow->changes[i];
        switch (c->type) {
            case CHG_INSERT: printf("  INSERT: %s (%d)\n", c->entity.name, c->entity.id); break;
            case CHG_UPDATE: printf("  UPDATE: %s (v%d)\n", c->entity.name, c->entity.version); break;
            case CHG_DELETE: printf("  DELETE: id=%d\n", c->entity.id); break;
        }
    }
    uow->count = 0;
    printf("[UoW] Done.\n");
}

int main(void) {
    UnitOfWork uow;
    uow_init(&uow);

    Entity alice = {1, "Alice", 1};
    Entity bob = {2, "Bob", 1};
    uow_register_new(&uow, alice);
    uow_register_new(&uow, bob);

    Entity alice2 = {1, "Alice Updated", 2};
    uow_register_dirty(&uow, alice2);
    uow_register_deleted(&uow, 2);

    uow_commit(&uow);
    return 0;
}
