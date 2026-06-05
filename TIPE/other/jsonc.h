#include <stdbool.h>

typedef struct Mott {
    int taille;
    char* mot;
} mott ;

mott* create_mott (int taille);

void add_char(mott* mot, char c);

bool compare(mott* mot, char* motif);
