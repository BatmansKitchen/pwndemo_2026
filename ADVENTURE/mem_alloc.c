#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

struct Chunk {
    uint8_t data[1024];
    struct Chunk* next;
};

struct Chunk* free_list = NULL;

void* mem_alloc(int size) {
    struct Chunk* c = free_list;
    if (c == NULL) {
        c = malloc(sizeof(struct Chunk));
    } else {
        free_list = c->next;
    }

    return c->data;
}

void mem_free(void* c) {
    struct Chunk* chunk = c;

    chunk->next = free_list;
    free_list = chunk;
}