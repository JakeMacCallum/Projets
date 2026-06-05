/* 
I'm numbering them as follows when facing the space (cube shape)
Right back top : 0          Left back top : 4
Right back bottom : 1       Left back bottom : 5
Right front top : 2         Left front top : 6
Right front bottom : 3      Left front bottom : 7
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "barnes-hut.h"
#include <stdbool.h>

#define G 0.00029591220828559093 // AU^3 / (M_sun * day^2)
#define EPSILON 0
#define THETA 0.5
#define MIN_H 1e-10
#define MAX_BODIES 10

const node empty_node = {0};


simulation* create_system(int N) {
    simulation* s = malloc(sizeof(simulation));
    s->N = N;
    s->bodies = malloc(N * sizeof(body));
    s->v = malloc(N * sizeof(vector));
    s->a = malloc(N * sizeof(vector));
    return s;
}

void init_system(simulation* s) {
    /* 
    preset Solar System on 30/06/2006, data from horizons
    in order Sun Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune
    units : au and days and solar mass
    origin : solar system barycenter @500
    */
   s->bodies[0] = (body){(vector){0.003243012229645923, 0.003256800382928769, -0.0001139286609168675}, 1};
   s->v[0] = (vector){-0.000004232828118191385, 0.000005234321825446492, 0.00000003499703848127283};
   
   s->bodies[1] = (body){(vector){-0.2021689112363301, -0.4117368948650247,-0.01516746039006004}, 0.00000016579};
   s->v[1] = (vector){0.01954520411149041, -0.01110839356091942, -0.002701962770220147};

      /* 2 = Venus */
   s->bodies[2] = (body){(vector){
       0.6804023807399970,
       0.2598394903799631,
      -0.03568610259702346}, 0.0000024446};
   s->v[2] = (vector){
      -0.007239159044565525,
       0.01883068196114596,
       0.0006752139400354907};

   /* 3 = Earth */
   s->bodies[3] = (body){(vector){
       0.1448643464735759,
      -1.003488618341284,
      -0.0001002787739701640}, 0.0000029995};
   s->v[3] = (vector){
       0.01675642091666775,
       0.002342291423915453,
       0.0000004858568001707811};

   /* 4 = Mars */
   s->bodies[4] = (body){(vector){
      -1.539057781127304,
       0.6310278663774521,
       0.05092737870785571}, 0.00000032228};
   s->v[4] = (vector){
      -0.004755169099420568,
      -0.01176078582060676,
      -0.0001297906413520642};

   /* 5 = Jupiter */
   s->bodies[5] = (body){(vector){
      -3.600631494293974,
      -4.030830494939988,
       0.09727751006002719}, 0.00095351};
   s->v[5] = (vector){
       0.005534702218923058,
      -0.004670923362979766,
      -0.0001044691613937274};

   /* 6 = Saturn */
   s->bodies[6] = (body){(vector){
      -6.271617328170606,
       6.647005106333681,
       0.1339250097430763}, 0.00028550};
   s->v[6] = (vector){
      -0.004354723899541339,
      -0.003842478968140932,
       0.0002403701424620918};

   /* 7 = Uranus */
   s->bodies[7] = (body){(vector){
      19.09231215844898,
      -6.225372626702382,
      -0.2704932232419811}, 0.000043604};
   s->v[7] = (vector){
       0.001190408125890519,
       0.003556088725384403,
      -0.000002285661948555561};

   /* 8 = Neptune */
   s->bodies[8] = (body){(vector){
      22.36117789978791,
     -20.08084213872839,
      -0.1018084661606435}, 0.000051445};
   s->v[8] = (vector){
       0.002077175723850884,
       0.002354292440820882,
      -0.00009631947789933795};
}


void free_system(simulation* s) {
    if (s->bodies != NULL) {
        free(s->bodies);
    } 
    if (s->v != NULL) {
        free(s->v);
    }
    if (s->a != NULL) {
        free(s->a);
    }
    free(s);
}

octree* create_octree(int N, double h) {
    octree* o = malloc(sizeof(octree));
    o->nodes = malloc(16 * N * sizeof(node));
    o->capacity = 8 * N;
    o->nodes[0] = new_node(h, (vector){0.0, 0.0, 0.0}, (vector){0.0, 0.0, 0.0});
    o->size = 1;
    return o;
}

void free_octree(octree* o) {
    for (int i = 0; i < o->size; i++) {
        if (o->nodes[i].stored_bodies != NULL) {
            free(o->nodes[i].stored_bodies);
        }
    }
    if (o->nodes != NULL) {
        free(o->nodes);
    }
    free(o);
}

void insert_node(octree* o, node n) {
    if (o->size == o->capacity - 1) {
        node* new = malloc(2 * o->capacity * sizeof(node));
        for (int i = 0 ; i < o->capacity ; i++) {
            new[i] = o->nodes[i];
        }
        o->capacity *= 2;
        free(o->nodes);
        o->nodes = new;
    }
    o->nodes[o->size] = n;
    o->size++;
}

void wipe_octree(octree* o, simulation* s) {
    vector min = s->bodies[0].position;
    vector max = s->bodies[0].position;
    for (int i = 1 ; i < s->N ; i++) {
        min.x = fmin(min.x, s->bodies[i].position.x);
        min.y = fmin(min.y, s->bodies[i].position.y);
        min.z = fmin(min.z, s->bodies[i].position.z);
        max.x = fmax(max.x, s->bodies[i].position.x);
        max.y = fmax(max.y, s->bodies[i].position.y);
        max.z = fmax(max.z, s->bodies[i].position.z);
    }
    double h = fmax(max.x - min.x, fmax(max.y - min.y, max.z - min.z)) * 2.2;
    if (h < MIN_H) h = 10*MIN_H;
    for (int i = 0 ; i < o->size ; i++) {
        o->nodes[i] = empty_node;
    }
    o->nodes[0] = new_node(h, (vector){0.0, 0.0, 0.0}, (vector){0.0, 0.0, 0.0});
    o->size = 1;
    return;
}

node new_node(double length, vector prev, vector u) {
    node root;
    root.center = (vector){prev.x + u.x * length / 2, prev.y + u.y * length / 2, prev.z + u.z * length / 2};
    root.h = length / 2;
    root.barycenter = (body){(vector){0.0, 0.0, 0.0}, 0.0};
    for (int i = 0 ; i < 8 ; i++) {
        root.children[i] = -1;
    }
    root.n_bodies = 0;
    root.stored_bodies = malloc(MAX_BODIES * sizeof(int));
    return root;

}

int get_quadrant(vector center, vector u) {
    return (
        (u.x > center.x ? 0 : 4) 
        + (u.y > center.y ? 0 : 2) 
        + (u.z > center.z ? 0 : 1)
    );
}

vector get_direction(vector center, vector u) {
    return (vector){
        (u.x > center.x ? 1 : -1),
        (u.y > center.y ? 1 : -1),
        (u.z > center.z ? 1 : -1)
    };
}

bool in_table (int* table, int element, int n) {
    for (int i = 0 ; i < n ; i++) {
        if (table[i] == element) return true;
    }
    return false;
}

void insert_body(octree* o, body* bodies, int node_i, int body_i) {
    body* b = &bodies[body_i];
    node* n = &o->nodes[node_i];
    if (n->n_bodies == 0) {
        n->barycenter.mass = b->mass;
        n->barycenter.position = b->position;
        n->n_bodies = 1;
        n->stored_bodies[0] = body_i;
        return;
    } else if (in_table(n->stored_bodies, body_i, n->n_bodies)) {
        return; // body is already stored in this quadrant
    } 
    int i = get_quadrant(n->center, b->position);
    double old = n->barycenter.mass;
    n->barycenter.mass += b->mass;
    n->barycenter.position.x = (n->barycenter.position.x * old + b->position.x * b->mass) / n->barycenter.mass;
    n->barycenter.position.y = (n->barycenter.position.y * old + b->position.y * b->mass) / n->barycenter.mass;
    n->barycenter.position.z = (n->barycenter.position.z * old + b->position.z * b->mass) / n->barycenter.mass;
    if (n->h < MIN_H) {
        n->stored_bodies[n->n_bodies] = body_i;
        n->n_bodies++;
        return;
    } 
    if (n->children[i] == -1) {
        n->children[i] = o->size;
        (void) insert_node(o, new_node(n->h, n->center, get_direction(n->center, b->position)));
    }
    if (n->n_bodies == 1) {
    // subdivide space to separate the two
        int stored_body = n->stored_bodies[0];
        int j = get_quadrant(n->center, bodies[stored_body].position);
        if (n->children[j] == -1) {
            n->children[j] = o->size;
            (void) insert_node(o, new_node(n->h, n->center, get_direction(n->center, bodies[stored_body].position)));
        }
        (void) insert_body(o, bodies, n->children[j], stored_body);
    }
    n->stored_bodies[n->n_bodies] = body_i;
    n->n_bodies++;
    (void) insert_body(o, bodies, n->children[i], body_i);
    return;
}

double dist(vector v1, vector v2) {
    double dx = (v1.x - v2.x);
    double dy = (v1.y - v2.y);
    double dz = (v1.z - v2.z);
    return sqrt(dx * dx + dy * dy + dz * dz);
}

void calculate_force(int subject_body, vector* forces, octree* o, body* bodies, int node_i) {
    node* n = &o->nodes[node_i];
    body* b1 = &bodies[subject_body];
    body* b2 = &o->nodes[node_i].barycenter;
    if (n->n_bodies == 0) { // empty node
        return;
    }
    if (n->h < MIN_H) {
        
    }
    double dx = (b2->position.x - b1->position.x);
    double dy = (b2->position.y - b1->position.y);
    double dz = (b2->position.z - b1->position.z);
    double sq_dist = dx * dx + dy * dy + dz * dz + EPSILON;
    double dist = sqrt(sq_dist);
    double magnitude = G * b1->mass * b2->mass / sq_dist;
    vector temp = (vector){magnitude * dx / dist, magnitude * dy / dist, magnitude * dz / dist};
    if (n->n_bodies == 1 && n->stored_bodies[0] != subject_body) { // single body and not subject body
        *forces = (vector){forces->x + temp.x, forces->y + temp.y, forces->z + temp.z};
    } else if (in_table(n->stored_bodies, subject_body, n->n_bodies)) {
       for (int i = 0 ; i < 8 ; i++) {
            if (n->children[i] != -1) {
                (void) calculate_force(subject_body, forces, o, bodies, n->children[i]);
            }
        }
    } else if ((n->h * 2.0 / dist) < THETA) {
        *forces = (vector){forces->x + temp.x, forces->y + temp.y, forces->z + temp.z};
    } else {
        for (int i = 0 ; i < 8 ; i++) {
            if (n->children[i] != -1) {
                (void) calculate_force(subject_body, forces, o, bodies, n->children[i]);
            }
        }
    }
    return;
}


void leapfrog_step(simulation* s, octree* o, double dt) {
    for (int i = 0 ; i < s->N ; i++) {
        s->v[i].x += s->a[i].x * (dt * 0.5);
        s->v[i].y += s->a[i].y * (dt * 0.5);
        s->v[i].z += s->a[i].z * (dt * 0.5);
    }
    for (int i = 0 ; i < s->N ; i++) {
        s->bodies[i].position.x += s->v[i].x * dt;
        s->bodies[i].position.y += s->v[i].y * dt;
        s->bodies[i].position.z += s->v[i].z * dt;
    }
    (void) wipe_octree(o, s);
    for (int i = 0 ; i < s->N ; i++) {
        insert_body(o, s->bodies, 0, i);
    }
    for (int i = 0 ; i < s->N ; i++) {
        vector f = {0.0, 0.0, 0.0};
        (void) calculate_force(i, &f, o, s->bodies, 0);
        double m = s->bodies[i].mass;
        if (m == 0.0) {
            s->a[i].x = s->a[i].y = s->a[i].z = 0.0;
        } else {
            s->a[i].x = f.x / m;
            s->a[i].y = f.y / m;
            s->a[i].z = f.z / m;
        }
    }
    for (int i = 0 ; i < s->N ; i++) {
        s->v[i].x += s->a[i].x * (dt * 0.5);
        s->v[i].y += s->a[i].y * (dt * 0.5);
        s->v[i].z += s->a[i].z * (dt * 0.5);
    }
}

void write_data(simulation* s, FILE** write) {
    for(int i = 0 ; i < s->N ; i++) {
        fprintf(write[i], ",%.15lf,%.15lf,%.15lf,%.15lf,%.15lf,%.15lf\n", 
        s->bodies[i].position.x,
        s->bodies[i].position.y,
        s->bodies[i].position.z,
        s->v[i].x,
        s->v[i].y,
        s->v[i].z
        );
    }
}
