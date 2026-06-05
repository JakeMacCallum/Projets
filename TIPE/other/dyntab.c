#define K 4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct DynArray {
  void* data;
  size_t elem_size;
  size_t size;
  size_t capacity;
} dynarray;

void init_dynarray(dynarray* a, size_t elem_size) {
  a->data = (void*)malloc(K * elem_size);
  a->elem_size = elem_size;
  a->size = 0;
  a->capacity = K;
}

void add_dynarray(dynarray* a, const void* elem) {
  if (a->size >= a->capacity) {
    void* new = (void*)malloc(2 * a->capacity * a->elem_size);
    memcpy(new, a->data, a->size * a->elem_size); 
    free(a->data);
    a->data = new;
    a->capacity *= 2;
  }
  memcpy((char*)a->data + a->elem_size * a->size, elem, a->elem_size);
  a->size++;
}

void* get_dynarray(dynarray *a, size_t idx) {
  if (idx >= a->size) return NULL;
  return (char*)a->data + idx * a->elem_size;
}

void free_dynarray(dynarray *a) {
  free(a->data);
  a->data = NULL;
  a->size = a->capacity = 0;
}
