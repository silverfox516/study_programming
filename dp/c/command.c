#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HISTORY 10

// Command interface
typedef struct Command {
    void (*execute)(struct Command* self);
    void (*undo)(struct Command* self);
    void (*destroy)(struct Command* self);
} Command;

// Receiver - Light
typedef struct Light {
    int is_on;
    char* location;
} Light;

Light* create_light(const char* location) {
    Light* light = malloc(sizeof(Light));
    light->is_on = 0;
    light->location = malloc(strlen(location) + 1);
    strcpy(light->location, location);
    return light;
}

void light_on(Light* light) {
    light->is_on = 1;
    printf("Light in %s is ON\n", light->location);
}

void light_off(Light* light) {
    light->is_on = 0;
    printf("Light in %s is OFF\n", light->location);
}

void destroy_light(Light* light) {
    if (light) {
        free(light->location);
        free(light);
    }
}

// Light On Command
typedef struct LightOnCommand {
    Command base;
    Light* light;
} LightOnCommand;

void light_on_execute(Command* cmd) {
    LightOnCommand* light_cmd = (LightOnCommand*)cmd;
    light_on(light_cmd->light);
}

void light_on_undo(Command* cmd) {
    LightOnCommand* light_cmd = (LightOnCommand*)cmd;
    light_off(light_cmd->light);
}

void light_on_destroy(Command* cmd) {
    free(cmd);
}

Command* create_light_on_command(Light* light) {
    LightOnCommand* cmd = malloc(sizeof(LightOnCommand));
    cmd->base.execute = light_on_execute;
    cmd->base.undo = light_on_undo;
    cmd->base.destroy = light_on_destroy;
    cmd->light = light;
    return (Command*)cmd;
}

// Light Off Command
typedef struct LightOffCommand {
    Command base;
    Light* light;
} LightOffCommand;

void light_off_execute(Command* cmd) {
    LightOffCommand* light_cmd = (LightOffCommand*)cmd;
    light_off(light_cmd->light);
}

void light_off_undo(Command* cmd) {
    LightOffCommand* light_cmd = (LightOffCommand*)cmd;
    light_on(light_cmd->light);
}

void light_off_destroy(Command* cmd) {
    free(cmd);
}

Command* create_light_off_command(Light* light) {
    LightOffCommand* cmd = malloc(sizeof(LightOffCommand));
    cmd->base.execute = light_off_execute;
    cmd->base.undo = light_off_undo;
    cmd->base.destroy = light_off_destroy;
    cmd->light = light;
    return (Command*)cmd;
}

// Invoker - Remote Control
typedef struct RemoteControl {
    Command* commands[7];  // 7 slots
    Command* undo_command;
    Command* history[MAX_HISTORY];
    int history_count;
} RemoteControl;

RemoteControl* create_remote_control() {
    RemoteControl* remote = malloc(sizeof(RemoteControl));
    for (int i = 0; i < 7; i++) {
        remote->commands[i] = NULL;
    }
    remote->undo_command = NULL;
    remote->history_count = 0;
    return remote;
}

void set_command(RemoteControl* remote, int slot, Command* command) {
    if (slot >= 0 && slot < 7) {
        remote->commands[slot] = command;
    }
}

void press_button(RemoteControl* remote, int slot) {
    if (slot >= 0 && slot < 7 && remote->commands[slot]) {
        remote->commands[slot]->execute(remote->commands[slot]);
        remote->undo_command = remote->commands[slot];

        // Add to history
        if (remote->history_count < MAX_HISTORY) {
            remote->history[remote->history_count++] = remote->commands[slot];
        } else {
            // Shift history
            for (int i = 0; i < MAX_HISTORY - 1; i++) {
                remote->history[i] = remote->history[i + 1];
            }
            remote->history[MAX_HISTORY - 1] = remote->commands[slot];
        }
    }
}

void press_undo(RemoteControl* remote) {
    if (remote->undo_command) {
        remote->undo_command->undo(remote->undo_command);
    }
}

void show_history(RemoteControl* remote) {
    printf("\n--- Command History ---\n");
    for (int i = 0; i < remote->history_count; i++) {
        printf("Command %d executed\n", i + 1);
    }
    printf("Total commands executed: %d\n\n", remote->history_count);
}

void destroy_remote_control(RemoteControl* remote) {
    if (remote) {
        for (int i = 0; i < 7; i++) {
            if (remote->commands[i]) {
                remote->commands[i]->destroy(remote->commands[i]);
            }
        }
        free(remote);
    }
}

int main() {
    printf("--- Smart Home Remote Control ---\n");

    // Create devices
    Light* living_room_light = create_light("Living Room");
    Light* kitchen_light = create_light("Kitchen");

    // Create commands
    Command* living_room_on = create_light_on_command(living_room_light);
    Command* living_room_off = create_light_off_command(living_room_light);
    Command* kitchen_on = create_light_on_command(kitchen_light);
    Command* kitchen_off = create_light_off_command(kitchen_light);

    // Create remote
    RemoteControl* remote = create_remote_control();

    // Set commands
    set_command(remote, 0, living_room_on);
    set_command(remote, 1, living_room_off);
    set_command(remote, 2, kitchen_on);
    set_command(remote, 3, kitchen_off);

    // Test remote
    printf("\nTesting remote control:\n");
    press_button(remote, 0);  // Living room on
    press_button(remote, 2);  // Kitchen on
    press_button(remote, 1);  // Living room off

    printf("\nTesting undo:\n");
    press_undo(remote);       // Undo living room off

    press_button(remote, 3);  // Kitchen off

    show_history(remote);

    // Cleanup
    destroy_light(living_room_light);
    destroy_light(kitchen_light);
    destroy_remote_control(remote);

    return 0;
}