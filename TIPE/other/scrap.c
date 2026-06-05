#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Settings {
  int n_steps;
  float theta;
  int dt;
  int scale;
  char* output_file;
  system S;
} settings;

typedef struct System {
  double* masss;
  vector* positions;
  vector* velocities;
} system;

typedef struct Vector {
  double x;
  double y;
  double z;
} vector;


typedef struct Variable {
  char* nom;
  typejsp val;
}
int main() {
    
    FILE *fptr = fopen("TIPE/settings/settings.json", "r");
  
    int charac = fgetc(fptr);
    int i = 1;
    while (charac != EOF && i < 207) {
        printf("%c", charac);
        charac = fgetc(fptr);
        i++;
    };
    
    fclose(fptr);
    return 0;
}