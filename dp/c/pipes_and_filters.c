// Pipes and Filters Pattern in C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*FilterFn)(int* data, int* size);

void filter_remove_negative(int* data, int* size) {
    int j = 0;
    for (int i = 0; i < *size; i++)
        if (data[i] >= 0) data[j++] = data[i];
    *size = j;
    printf("[Filter] Remove negatives -> %d items\n", *size);
}

void filter_double(int* data, int* size) {
    for (int i = 0; i < *size; i++) data[i] *= 2;
    printf("[Filter] Double values\n");
}

void filter_keep_under_100(int* data, int* size) {
    int j = 0;
    for (int i = 0; i < *size; i++)
        if (data[i] < 100) data[j++] = data[i];
    *size = j;
    printf("[Filter] Keep < 100 -> %d items\n", *size);
}

void filter_sort(int* data, int* size) {
    for (int i = 0; i < *size - 1; i++)
        for (int j = i + 1; j < *size; j++)
            if (data[i] > data[j]) { int t = data[i]; data[i] = data[j]; data[j] = t; }
    printf("[Filter] Sort\n");
}

typedef struct {
    FilterFn filters[16];
    int count;
} Pipeline;

void pipeline_add(Pipeline* p, FilterFn f) { p->filters[p->count++] = f; }

void pipeline_run(Pipeline* p, int* data, int* size) {
    for (int i = 0; i < p->count; i++)
        p->filters[i](data, size);
}

int main(void) {
    int data[] = {42, -5, 10, 80, 3, -1, 55, 25};
    int size = 8;

    Pipeline p = {.count = 0};
    pipeline_add(&p, filter_remove_negative);
    pipeline_add(&p, filter_double);
    pipeline_add(&p, filter_keep_under_100);
    pipeline_add(&p, filter_sort);

    pipeline_run(&p, data, &size);

    printf("Result:");
    for (int i = 0; i < size; i++) printf(" %d", data[i]);
    printf("\n");
    return 0;
}
