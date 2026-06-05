#include <stdio.h>
#include <stdbool.h>


typedef struct Vector
{
    double x, y, z;
}vector;

typedef struct Body
{
    vector position;
    double mass;
}body;

typedef struct Node
{
    vector center;
    double h; //half_length
    body barycenter;
    int children[8];
    int* stored_bodies;
    int n_bodies;
}node;

typedef struct Octree
{
    node* nodes; // root is 0 by construction
    int size;
    int capacity;
}octree;

typedef struct Simulation
{
    int N;
    body* bodies;
    vector* v;
    vector* a;
}simulation;

simulation* create_system(int N);

void init_system(simulation* s);

void free_system(simulation* s);

octree* create_octree(int N, double h);

void free_octree(octree* o);

void insert_node(octree* o, node n);

void wipe_octree(octree* o, simulation* s);

node new_node(double length, vector prev, vector u);

int get_quadrant(vector center, vector u);

vector get_direction(vector center, vector u);

void insert_body(octree* o, body* bodies, int node_i, int body_i);

double dist(vector v1, vector v2);

void calculate_force(int subject_body, vector* forces, octree* o, body* bodies, int node_i);

void leapfrog_step(simulation* s, octree* o, double dt);

void write_data(simulation* s, FILE** fptr);
