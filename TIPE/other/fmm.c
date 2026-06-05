#define OCTREE_INIT 64

void get_data(world* w);

typedef struct World {
  int N;
  double* pos[3];
  double* v[3];
  double* a[3];
  double* m;
} world;

world* init_world(int N) {
  world* w = (world*)malloc(sizeof(world));
  w->N = N;
  w->pos[0] = (double*)malloc(N * sizeof(double));
  w->pos[1] = (double*)malloc(N * sizeof(double));
  w->pos[2] = (double*)malloc(N * sizeof(double));
  w->v[0] = (double*)malloc(N * sizeof(double));
  w->v[1] = (double*)malloc(N * sizeof(double));
  w->v[2] = (double*)malloc(N * sizeof(double));
  w->a[0] = (double*)malloc(N * sizeof(double));
  w->a[1] = (double*)malloc(N * sizeof(double));
  w->a[2] = (double*)malloc(N * sizeof(double));
  w->m = (double*)malloc(N * sizeof(double));
  (void) get_data(w);
  return w;
}

void free_world(world* w) {
  free(w->pos[0]);
  free(w->pos[1]);
  free(w->pos[2]);
  free(w->v[0]);
  free(w->v[1]);
  free(w->v[2]);  
  free(w->v[0]);
  free(w->v[1]);
  free(w->v[2]);
  free(w->m);
  free(w);
  return;
}

typedef struct Node {
  double x;
  double y;
  double z;
  double m;
  double half_width;
} node;

typedef struct Octree {
  int size;
  int capacity;
  node* elements;
} octree;

octree* init_octree() {
  octree* o = (octree*)malloc(sizeof(octree));
  o->size = 0;
  o->capacity = OCTREE_INIT;
  o->elements = (node*)malloc(OCTREE_INIT * sizeof(int));
}

void free_octree(octree* o) {
  for (int i = 0 ; i < o->size ; ++i) {
    free(o->elements[i]);
  }
  free(o);
  return;
}

int get_quadrant(node* n, world* o, int i) {
/*
    Right back top : 0          Left back top : 4
    Right back bottom : 1       Left back bottom : 5
    Right front top : 2         Left front top : 6
    Right front bottom : 3      Left front bottom : 7
*/
  return (
      (o->x[i] > n->x ? 0 : 4) 
    + (o->y[i] > n->y ? 0 : 2) 
    + (o->z[i] > n->z ? 0 : 1)
  );
}

void insert_particle(world* w, octree* o, int i) {

}

void build_octree(world* w, octree* o) {
  for (int i = 0 ; i < w->N ; i++) {
    (void) insert_particle(w, o, i);
  }
  return;
}