// Active Object Pattern in C
// Decouples method execution from invocation using a message queue + worker thread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

typedef enum { CMD_ADD, CMD_MUL, CMD_SHUTDOWN } CmdType;

typedef struct {
    CmdType type;
    double a, b;
    double* result;
    pthread_mutex_t* done_mutex;
    pthread_cond_t* done_cond;
    bool* done_flag;
} Command;

typedef struct CmdNode {
    Command cmd;
    struct CmdNode* next;
} CmdNode;

typedef struct {
    CmdNode* head;
    CmdNode* tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t thread;
    bool running;
} ActiveObject;

void* ao_run(void* arg) {
    ActiveObject* ao = (ActiveObject*)arg;
    while (1) {
        pthread_mutex_lock(&ao->mutex);
        while (!ao->head && ao->running)
            pthread_cond_wait(&ao->cond, &ao->mutex);

        if (!ao->head && !ao->running) {
            pthread_mutex_unlock(&ao->mutex);
            break;
        }

        CmdNode* node = ao->head;
        ao->head = node->next;
        if (!ao->head) ao->tail = NULL;
        pthread_mutex_unlock(&ao->mutex);

        Command* cmd = &node->cmd;
        switch (cmd->type) {
            case CMD_ADD:
                *cmd->result = cmd->a + cmd->b;
                printf("[AO] %g + %g = %g\n", cmd->a, cmd->b, *cmd->result);
                break;
            case CMD_MUL:
                *cmd->result = cmd->a * cmd->b;
                printf("[AO] %g * %g = %g\n", cmd->a, cmd->b, *cmd->result);
                break;
            case CMD_SHUTDOWN:
                free(node);
                return NULL;
        }

        pthread_mutex_lock(cmd->done_mutex);
        *cmd->done_flag = true;
        pthread_cond_signal(cmd->done_cond);
        pthread_mutex_unlock(cmd->done_mutex);
        free(node);
    }
    return NULL;
}

ActiveObject* ao_create(void) {
    ActiveObject* ao = calloc(1, sizeof(ActiveObject));
    ao->running = true;
    pthread_mutex_init(&ao->mutex, NULL);
    pthread_cond_init(&ao->cond, NULL);
    pthread_create(&ao->thread, NULL, ao_run, ao);
    return ao;
}

double ao_invoke(ActiveObject* ao, CmdType type, double a, double b) {
    double result = 0;
    bool done = false;
    pthread_mutex_t dm = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t dc = PTHREAD_COND_INITIALIZER;

    CmdNode* node = malloc(sizeof(CmdNode));
    node->cmd = (Command){type, a, b, &result, &dm, &dc, &done};
    node->next = NULL;

    pthread_mutex_lock(&ao->mutex);
    if (ao->tail) ao->tail->next = node;
    else ao->head = node;
    ao->tail = node;
    pthread_cond_signal(&ao->cond);
    pthread_mutex_unlock(&ao->mutex);

    pthread_mutex_lock(&dm);
    while (!done) pthread_cond_wait(&dc, &dm);
    pthread_mutex_unlock(&dm);

    pthread_mutex_destroy(&dm);
    pthread_cond_destroy(&dc);
    return result;
}

void ao_destroy(ActiveObject* ao) {
    pthread_mutex_lock(&ao->mutex);
    ao->running = false;
    pthread_cond_signal(&ao->cond);
    pthread_mutex_unlock(&ao->mutex);
    pthread_join(ao->thread, NULL);
    pthread_mutex_destroy(&ao->mutex);
    pthread_cond_destroy(&ao->cond);
    free(ao);
}

int main(void) {
    ActiveObject* ao = ao_create();
    printf("3 + 4 = %g\n", ao_invoke(ao, CMD_ADD, 3, 4));
    printf("5 * 6 = %g\n", ao_invoke(ao, CMD_MUL, 5, 6));
    printf("10 + 20 = %g\n", ao_invoke(ao, CMD_ADD, 10, 20));
    ao_destroy(ao);
    return 0;
}
