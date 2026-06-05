#include <stdbool.h>
#include <math.h>

#define G 15000.0
#define SCALE 1 / 1000000


typedef struct Vector {
    double x;
    double y;
} vector ;


typedef struct Body {
    char* name;
    double mass;
    double radius;
    vector position;
    vector velocity;
    vector forces;
} body ;


typedef struct Systems {
    body* bodies;
    int size;
    double dt;
    bool gravitation;
} systems ;

body init_body(char* name, double mass, double radius, vector position, vector velocity, vector forces) ;

double distance(body* b1, body* b2) ;

void update_gravitation(body* b1, body* b2) ;

void update_body_forces(body* b1, body* b2, bool gravitation) ;



systems* init_system(int n, double dt, bool g, body* bodies) ;

void free_systems(systems* s) ;

void update_forces(systems* s) ;

void euler_update_positions(systems* s) ;


void verlet_update_positions(systems* s) ;

        