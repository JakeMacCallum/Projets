#include "my_complex.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    double x, y, z;
    double vx, vy, vz;
    double ax, ay, az;
    double ax2, ay2, az2;
    double m;
} particle;

typedef struct {
    double x;
    double y;
    double z;
    double l;

    complex_double** local;
    complex_double** multipole;

    int parent;
    int child[8];
    int level;

    int first_particle;
    int n_particles;

    int* colleagues;
    int n_colleagues;

    int* L1; 
    int n_L1;

    int* L2;
    int n_L2;

    int* L3;
    int n_L3;

    int* L4;
    int n_L4;
} box;

typedef struct {
    box* elements;
    int size;
    int capacity;

    int max_level;
    int** level_nodes;
    int* level_counts;
} octree;

typedef struct {
    particle* w;
    octree* o;
    double** a;
    double* fact;
} data;

double* init_fact();

double factorial(int n, double* fact);

octree* create_octree(double simulation_size, int N);

void postorder(octree* o, void* param, int current, void (*f)(int, void*, octree*));

void preorder(octree* o, void* param, int current, void (*f)(int, void*, octree*));

void free_node(int x, void* param, octree* o);

void free_octree(octree* o);

void check_size(octree* o);

bool in_box(particle part, box* b);

void swap(particle* world, int i, int j);

int max(int x, int y);

int min(int x, int y);

void recursive_build(octree* o, particle* world, int b_index);

octree* init_octree(particle* world, int N, double simulation_size);

particle* init_world(int N, double simulation_size);

double** calculate_a_coefficients(double* fact);

data* init_data(int N, double simulation_size);

void free_data(data* d);

void counter_tab_incr(int x, void* param, octree* o);

void fill_levels(int x, void* param, octree* o);

void build_level_array(octree* o);

bool are_colleagues(box* b, box* c);

void build_colleagues(octree* o);

bool well_separated(box* b, box* c);

void build_L2(octree* o);

double double_comp(double x, double y);

bool are_adjacent(box* b, box* c);

bool is_parent(box* b);

void L1_count(int i, void* y, octree* o);

void L1_add(int i, void* y, octree* o);

void build_L1(octree* o);

void L3_L4_count(octree* o, box* b, box* x);

void L3_add(octree* o, box* b, box* x);

void build_L3_L4(octree* o);

void step_0(octree* o);

double associated_legendre(int m, int l, double x, double* fact);

complex_double harmonic(int m, int n, double x, double y, double z, double r, double* fact);

complex_double multipole_coef(int n, int m, particle* world, box* b, double* fact);

complex_double** calculate_multipole_child(box* b, particle* world, double* fact);

complex_double formula_13(int k, int j, box* b, double** a, double* fact, octree* o);

complex_double** TMM(box* b, octree* o, double** a, double* fact);

void upward_pass(int i, void* x, octree* o);

double dist(particle* a, particle* b);

//theorem 2.2
complex_double local_coef(int* tab, int size, int k, int j, particle* world, octree* o, double* fact);

void calculate_local(int* tab, int size, box* b, particle* world, octree* o, double* fact);

complex_double formula_17(int* tab, int size, int k, int j, box* b, octree* o, double** a, double* fact);

void TML(int* tab, int size, box* b, octree* o, double** a, double* fact);

complex_double formula_21(int k, int j, box* b, box* c, double** a, double* fact);

void TLL(box* b, box* c, double** a, double* fact);

void downward_pass(int i, void* param, octree* o);

void fmm(data* d);

void update_positions(data* d, int N, double dt);

void reset_data(data* d, int N, double simulation_size);

void write_data(data* d, int N, FILE* file);


