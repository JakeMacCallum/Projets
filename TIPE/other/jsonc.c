#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*
typedef struct Mott {
    int taille;
    char* mot;
} mott ;

mott* create_mott (int taille) {
    mott* mot = malloc(sizeof(mot));
    mot->taille = taille;
    mot->mot = malloc(taille * sizeof(char));
    return mot;
}

void free_mott(mott* mot) {
    free(mot->mot);
    free(mot);
}

void add_char(mott* mot, char c) {
    for (int i = 1 ; i < mot->taille ; i++) {
        mot->mot[i-1] = mot->mot[i];
    }
    mot->mot[mot->taille] = c;
}

bool compare(mott* mot, char* motif) {
    for (int i = 0 ; i < mot->taille ; i++) {
        if (mot->mot[i] != motif[i]) return false;
    }
    return true;
}

*/
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING,
} ValueType;

typedef struct Entry {
    char* key;
    ValueType type;
    union {
        int i;
        double f;
        bool b;
        char* c;
    } value;
} entry;

typedef struct Storage {
    entry* entries;
    int max_size;
    int size;
} storage;

entry* create_entry(char* key, ValueType type, void* value) {
    entry* e = malloc(sizeof(entry));
    e->key = key;
    e->type = type;
    switch (type) {
        case TYPE_INT:
            e->value.i = *(int*)value;
            break;
        case TYPE_FLOAT:
            e->value.f = *(double*)value;
            break;
        case TYPE_BOOL:
            e->value.b = *(bool*)value;
            break;
        case TYPE_STRING:
            e->value.c = strdup((char*)value);
            break;
    }
    return e;
}

void free_entry(entry* e) {
    if (e->type == TYPE_STRING) {
        free(e->value.c);
    }
    free(e->key);
    free(e);
}

storage* create_storage(int size) {
    storage* s = malloc(sizeof(storage));
    s->entries = malloc(2*size*sizeof(entry));
    s->max_size = 2*size;
    s->size = size;
}

void free_storage(storage* s) {
    for (int i = 0 ; i < s->size ; i++) {
        free_entry(&s->entries[i]);
    }
    free(s->entries);
    free(s);
}