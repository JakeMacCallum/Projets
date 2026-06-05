#include "fmm.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define P 10 // order of multipole expansion
#define S 1

#define CUSHION 1e-8

#define PI 3.141592653589793
#define G 0.00029591220828559093 // AU^3 / (M_sun * day^2)
//#define G 6.6743*1e-11

#define OCTREE_INIT 64
#define GEOM_EPSILON 1e-10
#define FACT_SIZE 50 // max 20 for int64

#define UNUSED(x) (void)x

double* init_fact() {
    double* tab = (double*)malloc(FACT_SIZE * sizeof(double));
    if (tab == NULL) {
        perror("Malloc Failed init_fact");
    }
    for (int i = 0 ; i < FACT_SIZE ; ++i) tab[i] = -1;
    tab[0] = 1;
    tab[1] = 1;
    return tab;
}

double factorial(int n, double* fact) {
    if (n <= 1) {
        return 1;
    }
    else if (n >= FACT_SIZE) {
        perror("Tableau trop petit");
        return -1;
    }
    else {
        if (fact[n] != -1) {
            return fact[n];
        }
        else {
            int i = n;
            while (fact[i] == -1) {
                --i;
            }
            for (int j = i+1 ; j < n + 1 ; ++j) {
                fact[j] = j*fact[j-1];
            }
            return fact[n];
        }
    }
}

octree* create_octree(double simulation_size, int N) {
    octree* o = (octree*)malloc(sizeof(octree));
    o->size = 1;
    o->capacity = OCTREE_INIT;
    o->elements = (box*)malloc(OCTREE_INIT * sizeof(box));
    o->elements[0] = (box){
        .x = 0, 
        .y = 0, 
        .z = 0, 
        .l = simulation_size,
        .parent = 0, 
        .child = {-1,-1,-1,-1,-1,-1,-1,-1}, 
        .level = 0,
        .first_particle = 0, 
        .n_particles = N,
        .local = NULL,
        .multipole = NULL,
        .colleagues = NULL,
        .L1 = NULL,
        .L2 = NULL,
        .L3 = NULL,
        .L4 = NULL,
        .n_colleagues = 0,
        .n_L1 = 0,
        .n_L2 = 0,
        .n_L3 = 0,
        .n_L4 = 0,
    };
    o->max_level = 0;
    o->level_nodes = NULL;
    o->level_counts = NULL;
    return o;
}

void postorder(octree* o, void* param, int current, void (*f)(int, void*, octree*)) {
    for (int i = 0 ; i < 8 ; ++i) {
        if (o->elements[current].child[i] != -1) {
            postorder(o, param, o->elements[current].child[i], *f); 
        }
    }
    (*f)(current, param, o);
    return;
}

void preorder(octree* o, void* param, int current, void (*f)(int, void*, octree*)) {
    (*f)(current, param, o);
    for (int i = 0 ; i < 8 ; ++i) {
        if (o->elements[current].child[i] != -1) {
            preorder(o, param, o->elements[current].child[i], *f); 
        }
    }
    return;
}

void free_node(int x, void* param, octree* o) {
    UNUSED(param);
    box* c = &o->elements[x];
    if (c->local != NULL) {
        for (int i = 0 ; i < P + 1 ; ++i) {
            if (c->local[i] != NULL) free(c->local[i]);
        }
        free(c->local);
    }
    if (c->multipole != NULL) {
        for (int i = 0 ; i < P + 1 ; ++i) {
            if (c->multipole[i] != NULL) free(c->multipole[i]);
        }
        free(c->multipole);
    }
    if (c->colleagues != NULL) free(c->colleagues);
    if (c->L1 != NULL) free(c->L1);
    if (c->L2 != NULL) free(c->L2);
    if (c->L3 != NULL) free(c->L3);
    if (c->L4 != NULL) free(c->L4);
    return;
}

void free_octree(octree* o) {
    if (o != NULL) {
        postorder(o, NULL, 0, free_node);
        if (o->level_nodes != NULL) {
            for (int i = 0 ; i < o->max_level + 1 ; ++i) {
                if (o->level_nodes[i] != NULL) free(o->level_nodes[i]);
            }
            free(o->level_nodes);
        }
        if (o->elements != NULL) free(o->elements);
        if (o->level_counts != NULL) free(o->level_counts);
        free(o);
    }
    return;
}

void check_size(octree* o) {
    if (o->size >= o->capacity) {
        box* new = (box*)malloc(2 * o->capacity * sizeof(box));
        o->capacity *= 2;
        for (int i = 0 ; i < o->size ; ++i) {
            new[i] = o->elements[i];
        }
        free(o->elements);
        o->elements = new;
    }
    return;
}

bool in_box(particle part, box* b) {
    double half = b->l * 0.5;
    return
    part.x <= (b->x + half) && part.x >= (b->x - half) &&
    part.y <= (b->y + half) && part.y >= (b->y - half) &&
    part.z <= (b->z + half) && part.z >= (b->z - half);
}

void swap(particle* world, int i, int j) {
    if (i == j) return;
    particle temp = world[i];
    world[i] = world[j];
    world[j] = temp;
    return;
}

int max(int x, int y) {
    return (x <= y) * y + (x > y) * x;
}

int min(int x, int y) {
    return (x<y)*x + (x>=y)*y;
}

void recursive_build(octree* o, particle* world, int b_index) {
    box* b = &o->elements[b_index];
    if (b->l < GEOM_EPSILON) return;
    // Find all particles in b and put them in [first_particle, first_particle + n_particles[
    int N = o->elements[0].n_particles;
    if (b->n_particles != N) {
        for (int i = b->first_particle ; i < N ; ++i) {
            if (in_box(world[i], b)) {
                swap(world, i, b->first_particle + b->n_particles);
                ++b->n_particles;
            }
        }
    }
    //printf("%d\n", b->n_particles);
    // S is max number of particles in a leaf 
    if (b->n_particles > S) {
        double h = b->l * 0.5;
        double q = b->l * 0.25;
        int offset = 0;
        for (int k = 0; k < 8; ++k) {
            check_size(o);
            b = &o->elements[b_index];

            int child_index = o->size;
            b->child[k] = child_index;
            box* c = &o->elements[child_index];
            ++o->size;

            c->x = b->x + ((k & 1) ? q : -q);
            c->y = b->y + ((k & 2) ? q : -q);
            c->z = b->z + ((k & 4) ? q : -q);
            c->l = h;
            for (int t = 0 ; t < 8 ; ++t) {
                c->child[t] = -1;
            }
            c->first_particle = b->first_particle + offset;
            c->n_particles = 0;
            c->parent = b - o->elements;
            c->level = b->level + 1;
            c->n_colleagues = 0;
            c->colleagues = NULL;
            c->n_L1 = 0;
            c->L1 = NULL;
            c->n_L2 = 0;
            c->L2 = NULL;
            c->n_L3 = 0;
            c->L3 = NULL;
            c->n_L4 = 0;
            c->L4 = NULL;
            c->local = NULL;
            c->multipole = NULL;

            (void)recursive_build(o, world, child_index);

            b = &o->elements[b_index];
            c = &o->elements[child_index];

            if (c->n_particles == 0) {
                b->child[k] = -1;
                --o->size;
            }
            else {
                o->max_level = max(o->max_level, c->level);
                offset += c->n_particles;                
            }
        }
    }
    return;
}

octree* init_octree(particle* world, int N, double simulation_size) {
    octree* o = create_octree(simulation_size, N);
    recursive_build(o, world, 0);
    return o;
}

particle* init_world(int N, double simulation_size) { // arbitrary
    particle* world = (particle*)malloc(N * sizeof(particle));
    int d = (int)(simulation_size/5);
    int vd = (int)(simulation_size/5000);
    if (N == 2) {
        world[0] = (particle){
                .x = 3, .y = 0, .z = 0,
                .vx = 0, .vy = sqrt(G/12.0), .vz = 0,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 1
            };
        world[1] = (particle){
            .x = -3, .y = 0, .z = 0,
            .vx = 0, .vy = -sqrt(G/12.0), .vz = 0,
            .ax = 0, .ay = 0, .az = 0,
            .ax2 = 0, .ay2 = 0, .az2 = 0,
            .m = 1
        };
    }
    else if (N == 9) {
        world[0] = (particle){
                .x = 0.003243012229645923, .y = 0.003256800382928769, .z = -0.0001139286609168675,
                .vx = -0.000004232828118191385, .vy = 0.000005234321825446492, .vz = 0.00000003499703848127283,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 1.0
            };
        world[1] = (particle){
                .x = -0.2021689112363301, .y = -0.4117368948650247, .z = -0.01516746039006004,
                .vx = 0.01954520411149041, .vy = -0.01110839356091942, .vz = -0.002701962770220147,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 0.00000016579
            };
        world[2] = (particle){
                .x = 0.6804023807399970, .y = 0.2598394903799631, .z = -0.03568610259702346,
                .vx = -0.007239159044565525, .vy = 0.01883068196114596, .vz = 0.0006752139400354907,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 0.0000024446
            };
        world[3] = (particle){
                .x = 0.1448643464735759, .y = -1.003488618341284, .z = -0.0001002787739701640,
                .vx = 0.01675642091666775, .vy = 0.002342291423915453, .vz = 0.0000004858568001707811,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 0.0000029995
            };
        world[4] = (particle){
                .x = -1.539057781127304, .y = 0.6310278663774521, .z = 0.05092737870785571,
                .vx = -0.004755169099420568, .vy = -0.01176078582060676, .vz = -0.0001297906413520642,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 0.00000032228
            };
        world[5] = (particle){
                .x = -3.600631494293974, .y = -4.030830494939988, .z = 0.09727751006002719,
                .vx = 0.005534702218923058, .vy = -0.004670923362979766, .vz = -0.0001044691613937274,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 0.00095351
            };
        world[6] = (particle){
                .x = -6.271617328170606, .y = 6.647005106333681, .z = 0.1339250097430763,
                .vx = -0.004354723899541339, .vy = -0.003842478968140932, .vz = 0.0002403701424620918,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 0.00028550
            };
        world[7] = (particle){
                .x = 19.09231215844898, .y = -6.225372626702382, .z = -0.2704932232419811,
                .vx = 0.001190408125890519, .vy = 0.003556088725384403, .vz = -0.000002285661948555561,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 0.000043604
            };
        world[8] = (particle){
                .x = 22.36117789978791, .y = -20.08084213872839, .z = -0.1018084661606435,
                .vx = 0.002077175723850884, .vy = 0.002354292440820882, .vz = -0.00009631947789933795,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = 0.000051445
            };
    }
    else {
        for (int l = 0 ; l < N ; ++l) {
            world[l] = (particle){
                .x = (rand() % (2*d)) - d, .y = (rand() % (2*d)) - d, .z = (rand() % (2*d)) - d,
                .vx = (rand() % (2*vd)) - vd, .vy = (rand() % (2*vd)) - vd, .vz = (rand() % (2*vd)) - vd,
                .ax = 0, .ay = 0, .az = 0,
                .ax2 = 0, .ay2 = 0, .az2 = 0,
                .m = (rand() % 100) - 50
            };
        }
    }
    return world;
}

double** calculate_a_coefficients(double* fact) {
    double** a = (double**)malloc((2*P+1) * sizeof(double*));
    for (int n = 0 ; n < 2*P+1 ; ++n) {
        a[n] = (double*)malloc((2*n+1) * sizeof(double));
        int e = power(-1, n);
        for (int m = -n ; m < n + 1 ; ++m) {
            a[n][m+n] = e / sqrt(factorial(n-m, fact)*factorial(n+m, fact));
        }
    }
    return a;
}

data* init_data(int N, double simulation_size) {
    data* d = (data*)malloc(sizeof(data));
    d->w = init_world(N, simulation_size);
    d->o = init_octree(d->w, N, simulation_size);
    d->fact = init_fact();
    d->a = calculate_a_coefficients(d->fact);
    return d;
}

void free_data(data* d) {
    free(d->w);
    free_octree(d->o);
    free(d->fact);
    for (int i = 0 ; i < 2*P + 1 ; ++i) {
        free(d->a[i]);
    }
    free(d->a);
    free(d);
    return;
}

void counter_tab_incr(int x, void* param, octree* o) {
    UNUSED(param);
    o->level_counts[o->elements[x].level] += 1;
    return;
}

void fill_levels(int x, void* param, octree* o) {
    UNUSED(param);
    int l = o->elements[x].level;
    o->level_nodes[l][o->level_counts[l]] = x;
    ++o->level_counts[l];
    return;
}

void build_level_array(octree* o) {
    int n = o->max_level + 1;
    o->level_nodes = (int**)malloc(n * sizeof(int*));
    o->level_counts = (int*)malloc(n * sizeof(int));
    for (int i = 0 ; i < n ; ++i) o->level_counts[i] = 0;
    postorder(o, NULL, 0, counter_tab_incr);
    for (int i = 0 ; i < n ; ++i) {
        o->level_nodes[i] = (int*)malloc(o->level_counts[i] * sizeof(int));

        o->level_counts[i] = 0;
    }
    postorder(o, NULL, 0, fill_levels);
}

bool are_colleagues(box* b, box* c) {
    double l = b->l;
    return 
        fabs(b->x - c->x) <= l *(1 + 1e-12) &&
        fabs(b->y - c->y) <= l *(1 + 1e-12) &&
        fabs(b->z - c->z) <= l *(1 + 1e-12) &&
        b->level == c->level;
}

void build_colleagues(octree* o) {
    for (int l = 1 ; l < o->max_level + 1 ; ++l) {
        for (int i = 0 ; i < o->level_counts[l] ; ++i) {
            int x = o->level_nodes[l][i];
            for (int j = i ; j < o->level_counts[l] ; ++j) {
                int y = o->level_nodes[l][j];
                if (are_colleagues(o->elements + x, o->elements + y)) {
                    ++o->elements[x].n_colleagues;
                    ++o->elements[y].n_colleagues;
                }
            }

            o->elements[x].colleagues = (int*)malloc(o->elements[x].n_colleagues * sizeof(int));
            o->elements[x].n_colleagues = 0;

            for (int j = 0 ; j < o->level_counts[l] ; ++j) {
                int y = o->level_nodes[l][j];
                if (are_colleagues(o->elements + x, o->elements + y)) {
                    o->elements[x].colleagues[o->elements[x].n_colleagues] = y;
                    ++o->elements[x].n_colleagues;
                }
            }
        }
    }
    return;
}

bool well_separated(box* b, box* c) {
    return ! are_colleagues(b, c);
}

void build_L2(octree* o) {
    for (int l = 1 ; l < o->max_level + 1 ; ++l) {
        for (int i = 0 ; i < o->level_counts[l] ; ++i) {
            int x = o->level_nodes[l][i];
            box* parent = &o->elements[o->elements[x].parent];
            for (int j = 0 ; j < parent->n_colleagues ; ++j) {
                box* neighbor = &o->elements[parent->colleagues[j]];
                for (int k = 0 ; k < 8 ; ++k) {
                    if (neighbor->child[k] != -1 && well_separated(o->elements + x, o->elements + neighbor->child[k])) {
                        ++o->elements[x].n_L2;
                    }
                }
            }
            o->elements[x].L2 = (int*)malloc(o->elements[x].n_L2 * sizeof(int));
            o->elements[x].n_L2 = 0;
            for (int j = 0 ; j < parent->n_colleagues ; ++j) {
                box* neighbor = &o->elements[parent->colleagues[j]];
                for (int k = 0 ; k < 8 ; ++k) {
                    if (neighbor->child[k] != -1 && well_separated(o->elements + x, o->elements + neighbor->child[k])) {
                        o->elements[x].L2[o->elements[x].n_L2] = neighbor->child[k];
                        ++o->elements[x].n_L2;
                    }
                }
            }
        }
    }
    return;
}

double double_comp(double x, double y) {
// returns -1 if x < y, 1 if x > y and 0 otherwise
    return (x > y) - (x < y);
}

bool are_adjacent(box* b, box* c) {
    double limit = (b->l + c->l) * 0.5;
    return 
        fabs(b->z - c->z) <= limit *(1 + 1e-12) &&
        fabs(b->y - c->y) <= limit *(1 + 1e-12) &&
        fabs(b->x - c->x) <= limit *(1 + 1e-12);
}

bool is_parent(box* b) {
    for (int i = 0 ; i < 8 ; ++i) {
        if (b->child[i] != -1) return true;
    }
    return false;
}

void L1_count(int i, void* y, octree* o) {
    box* x = (box*)y;
    box* b = &o->elements[i];
    if (!is_parent(b)) {
        if (are_adjacent(b, x)) {
            ++x->n_L1;
        }
    }
    return;
}

void L1_add(int i, void* y, octree* o) {
    box* x = (box*)y;
    box* b = o->elements + i;
    if (!is_parent(b) && b != x) {
        if (are_adjacent(b, x)) {
            x->L1[x->n_L1] = i;
            ++x->n_L1;
        }
    }
    return;
}

void build_L1(octree* o) {
    for (int i = 0 ; i < o->size ; ++i) {
        box* x = &o->elements[i]; 
        if (!is_parent(x)) {
            postorder(o, x, 0, L1_count);
            x->L1 = (int*) malloc(x->n_L1 * sizeof(int));
            x->n_L1 = 0;
            postorder(o, x, 0, L1_add);
        }
    }
    return;
}

void L3_L4_count(octree* o, box* b, box* x) {
    if (are_adjacent(b, x)) {
        for (int i = 0 ; i < 8 ; ++i) {
            if (b->child[i] != -1) {
                box* c = &o->elements[b->child[i]];
                if (!are_adjacent(c, x)) {
                    ++x->n_L3;
                    ++c->n_L4;
                }
                else {
                    L3_L4_count(o, c, x);
                }
            }
        }
    }
    return;
}

void L3_add(octree* o, box* b, box* x) {
    if (are_adjacent(b, x)) {
        for (int i = 0 ; i < 8 ; ++i) {
            if (b->child[i] != -1) {
                box* c = o->elements + b->child[i];
                if (!are_adjacent(c, x)) {
                    x->L3[x->n_L3] = b->child[i];
                    ++x->n_L3;
                }
                else {
                    L3_add(o, c, x);
                }
            }
        }
    }
    return;
}

void build_L3_L4(octree* o) {
    for (int i = 0 ; i < o->size ; ++i) {
        box* x = &o->elements[i];
        if (!is_parent(x)) {
            for (int j = 0 ; j < x->n_colleagues ; ++j) {
                L3_L4_count(o, &o->elements[x->colleagues[j]], x);
            }
            x->L3 = (int*)malloc(x->n_L3 * sizeof(int));
            x->n_L3 = 0;
            for (int j = 0 ; j < x->n_colleagues ; ++j) {
                L3_add(o, &o->elements[x->colleagues[j]], x);
            }
        }
    }
    for (int i = 0 ; i < o->size ; ++i) {
        box* x = &o->elements[i];
        if (x->n_L4 != 0) {
            x->L4 = (int*)malloc(x->n_L4 * sizeof(int));
            x->n_L4 = 0;
        }
    }
    for (int i = 0 ; i < o->size ; ++i) {
        box* x = &o->elements[i];
        for (int j = 0 ; j < x->n_L3 ; ++j) {
            box* c = &o->elements[x->L3[j]];
            c->L4[c->n_L4] = i;
            ++c->n_L4;
        }
    }
    return;
}

void step_0(octree* o) {
    (void) build_level_array(o);
    (void) build_colleagues(o);
    (void) build_L2(o);
    (void) build_L1(o);
    (void) build_L3_L4(o);
    return;
}


double associated_legendre(int m, int l, double x, double* fact) {
    if (fabs(x) > 1) perror("legendre |x|>1");
    //https://justinwillmert.com/articles/2020/calculating-legendre-polynomials/
    double p = 1;
    if (m < 0) {
        int f = factorial(m-l, fact);
        return (1 - 2 * (-m % 2)) * factorial(l+m, fact) * associated_legendre(-m, l, x, fact) / f;
    }
    else if (m == 0 && l == 0) {
        return 1;
    }
    else {
        double y = sqrt(1 - x*x);
        for (int k = 0 ; k < m ; ++k) {
            p *= -(2*k+1)*y;
        }
        if (l == m) {
            return p;
        }
        else {
            double pmin2 = p;
            double pmin1 = (2*m+1) * x * pmin2;
            if (l == m+1) {
                return pmin1;
            }
            else {
                for (int i = m+1 ; i < l ; ++i) {
                    p = ((2*i+1) * x * pmin1 - (i + m) * pmin2) / (i - m + 1);
                    pmin2 = pmin1;
                    pmin1 = p;
                }
                return p;
            }
        }
    }
}

complex_double harmonic(int m, int n, double x, double y, double z, double r, double* fact) {
    complex_double val = e_i_m_theta(x, y, m);
    double p = associated_legendre(abs(m), n, z/r, fact)
      * sqrt(
            (2*n+1)*factorial(n-abs(m), fact)
            / (4*PI*factorial(n+abs(m), fact))
        );
    return multiply_real(val, p);
}

complex_double multipole_coef(int n, int m, particle* world, box* b, double* fact) {
    complex_double sum = zero();
    int fst = b->first_particle;
    int n_p = b->n_particles;
    for (int i = fst ; i < n_p + fst ; ++i) {
        double x = world[i].x - b->x;
        double y = world[i].y - b->y;
        double z = world[i].z - b->z;
        double r = sqrt(x*x + y*y + z*z);
        double rho = power(r, n);
        double val = world[i].m * rho;
        sum = add_complex(sum, multiply_real(harmonic(-m, n, x, y, z, r, fact), val));
    }
    return sum;
}

complex_double** calculate_multipole_child(box* b, particle* world, double* fact) {
//theorem 2.1
    complex_double** multipole = (complex_double**)malloc((P+1) * sizeof(complex_double*));
    for (int n = 0 ; n <= P ; ++n) {
        multipole[n] = (complex_double*)malloc((2*n+1) * sizeof(complex_double));
        for (int m = -n ; m < n + 1 ; ++m) {
            multipole[n][m+n] = multipole_coef(n, m, world, b, fact);
        }
    }
    return multipole;
}

complex_double formula_13(int k, int j, box* b, double** a, double* fact, octree* o) {
    complex_double sum = zero();
    for (int i = 0 ; i < 8 ; ++i) {
        if (b->child[i] != -1) {
            box* c = &o->elements[b->child[i]];
            double x = c->x - b->x;
            double y = c->y - b->y;
            double z = c->z - b->z;
            for (int n = 0 ; n < j + 1 ; ++n) {
                int l = j - n;
                int m_min = max(-n, k - l);
                int m_max = min(n, k + l);
                for (int m = m_min; m <= m_max; ++m) {
                    int mu = k - m;
                    complex_double coef = i_power(abs(k) - abs(m) - abs(k-m));
                    coef = multiply_real(coef, a[n][m+n] * a[j-n][mu + l] / a[j][k+j]);
                    double r = sqrt(x*x + y*y + z*z);
                    double rho = power(r, n);
                    coef = multiply_real(multiply_complex(coef, c->multipole[j-n][mu + l]), rho);
                    sum = add_complex(sum, multiply_complex(coef, harmonic(-m, n, x, y, z, r, fact)));
                }
            }
        }
    }
    return sum;
}

complex_double** TMM(box* b, octree* o, double** a, double* fact) {
    complex_double** multipole = (complex_double**)malloc((P+1) * sizeof(complex_double*));
    for (int n = 0 ; n <= P ; ++n) {
        multipole[n] = (complex_double*)malloc((2*n+1)*sizeof(complex_double));
        for (int m = -n ; m <= n ; ++m) {
            multipole[n][m+n] = formula_13(m, n, b, a, fact, o);
        }
    }
    return multipole;
}

void upward_pass(int i, void* x, octree* o) {
    data* d = (data*)x;
    particle* world = d->w;
    double** a = d->a;
    double* fact = d->fact;
    box* b = &o->elements[i];
    if (is_parent(b)) { // step 2
        b->multipole = TMM(b, o, a, fact);
    }
    else { // step 1
        b->multipole = calculate_multipole_child(b, world, fact); 
    }
    return;
}

double dist(particle* a, particle* b) {
    double dx = a->x - b->x;
    double dy = a->y - b->y;
    double dz = a->z - b->z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

//theorem 2.2
complex_double local_coef(int* tab, int size, int k, int j, particle* world, octree* o, double* fact) {
    complex_double sum = zero(); 
    for (int i = 0 ; i < size ; ++i) {
        box* b = &o->elements[tab[i]];
        for (int l = 0 ; l < b->n_particles ; ++l) {
            particle* part = &world[b->first_particle + l];
            double x = part->x - b->x;
            double y = part->y - b->y;
            double z = part->z - b->z;
            double r = sqrt(x*x + y*y + z*z);
            sum = add_complex(sum, multiply_real(harmonic(-k, j, x, y, z, r, fact), part->m / power(r, j+1)));
        }
    }
    return sum;
}

void calculate_local(int* tab, int size, box* b, particle* world, octree* o, double* fact) {
    for (int j = 0 ; j <= P ; ++j) {
        for (int k = -j ; k <= j ; ++k) {
            b->local[j][k+j] = add_complex(b->local[j][k+j], local_coef(tab, size, k, j, world, o, fact));
        }
    }
}

complex_double formula_17(int* tab, int size, int k, int j, box* b, octree* o, double** a, double* fact) {
    complex_double sum = zero();
    for (int n = 0 ; n < P + 1 ; ++n) {
        for (int m = -n ; m < n + 1 ; ++m) {
            complex_double coef = i_power(abs(k-m) - abs(k) - abs(m));
            coef = multiply_real(coef, a[n][m+n] * a[j][k+j] / a[j+n][m-k+n+j] * power(-1, n));
            for (int i = 0 ; i < size ; ++i) {
                box* c = &o->elements[tab[i]];
                complex_double coef2 = multiply_complex(coef, c->multipole[n][m+n]);
                double x = c->x - b->x;
                double y = c->y - b->y;
                double z = c->z - b->z;
                double r = sqrt(x*x + y*y + z*z);
                coef2 = multiply_complex(coef2, harmonic(m-k, j+n, x, y, z, r, fact));
                sum = add_complex(sum, multiply_real(coef2, 1 / power(r, j+n+1)));
            }
        }
    }
    return sum;
}

void TML(int* tab, int size, box* b, octree* o, double** a, double* fact) {
    for (int j = 0 ; j <= P ; ++j) {
        for (int k = -j ; k < j + 1 ; ++k) {
            b->local[j][k+j] = add_complex(b->local[j][k+j], formula_17(tab, size, k, j, b, o, a, fact));
        }
    }
}

complex_double formula_21(int k, int j, box* b, box* c, double** a, double* fact) {
    complex_double sum = zero();
    double x = c->x - b->x;
    double y = c->y - b->y;
    double z = c->z - b->z;
    double r = sqrt(x*x + y*y + z*z);
    for (int n = j ; n < P + 1 ; ++n) {
        int m_min = max(-n, k - (n-j));
        int m_max = min(n, k + n-j);
        for (int m = m_min ; m < m_max + 1 ; ++m) {
            complex_double coef = i_power(abs(m) - abs(k-m) - abs(k));
            coef = multiply_real(coef, a[n-j][m-k + n-j] * a[j][k+j] / a[n][m+n] * power(-1, n+j));
            coef = multiply_complex(coef, multiply_complex(b->local[j][k+j], harmonic(m-k, n-j, x, y, z, r, fact)));
            sum = add_complex(sum, multiply_real(coef, power(r, n-j)));
        }
    }
    return sum;
}

void TLL(box* b, box* c, double** a, double* fact) {
    for (int j = 0 ; j < P + 1 ; ++j) {
        for (int k = -j ; k < j + 1 ; ++k) {
            c->local[j][k+j] = formula_21(k, j, b, c, a, fact);
        }
    }
}

void downward_pass(int i, void* param, octree* o) {
    data* d = (data*)param;
    particle* world = d->w;
    double** a = d->a;
    double* fact = d->fact;
    box* b = &o->elements[i];
    if (i == 0) {
        for (int j = 0 ; j < 8 ; ++j) {
            if (b->child[j] != -1) {
                box* c = &o->elements[b->child[j]];
                c->local = (complex_double**)malloc((P+1)*sizeof(complex_double*));
                for (int k = 0 ; k < P + 1 ; ++k) {
                    c->local[k] = (complex_double*)malloc((2*k+1)*sizeof(complex_double));
                    for (int l = 0 ; l < 2*k+1 ; ++l) {
                        c->local[k][l] = zero();
                    }
                }
            }
        }
    } 
    else {
        //step 3 -> step 4 assuming tables exist
        if (b->n_particles <= P*P) {
            for (int j = 0 ; j < b->n_L4 ; ++j) {
                box* c = &o->elements[b->L4[j]];
                for (int k = 0 ; k < b->n_particles ; ++k) {
                    particle* b_part = world + b->first_particle + k;
                    for (int l = 0 ; l < c->n_particles ; ++l) {
                        particle* c_part = world + c->first_particle + l;
                        double dx = c_part->x - b_part->x;
                        double dy = c_part->y - b_part->y;
                        double dz = c_part->z - b_part->z;

                        double r2 = dx*dx + dy*dy + dz*dz + CUSHION*CUSHION;
                        double invr = 1.0/sqrt(r2);
                        double invr3 = invr*invr*invr;

                        b_part->ax += G * c_part->m * dx * invr3;
                        b_part->ay += G * c_part->m * dy * invr3;
                        b_part->az += G * c_part->m * dz * invr3;
                    }
                }
            }
        }   
        else {
            (void)calculate_local(b->L4, b->n_L4, b, world, o, fact);
        }
        (void)TML(b->L2, b->n_L2, b, o, a, fact); // step 4
        if (is_parent(b)) { //step 5 and init children
            for (int t = 0 ; t < 8 ; ++t) {
                if (b->child[t] != -1) {
                    box* c = &o->elements[b->child[t]];
                    c->local = (complex_double**)malloc((P+1) * sizeof(complex_double*));
                    for (int j = 0 ; j < P + 1 ; ++j) {
                        c->local[j] = (complex_double*)malloc((2*j+1) * sizeof(complex_double));
                        for (int l = 0 ; l < 2*j+1 ; ++l) {
                            c->local[j][l] = zero();
                        }
                    }
                    TLL(b, c, a, fact); //shift to children
                }
            }
        }
        else {
            for (int l = 0 ; l < b->n_particles ; ++l) {
                particle* part = &world[b->first_particle + l];
                double x = part->x - b->x;
                double y = part->y - b->y;
                double z = part->z - b->z;
                double r = sqrt(x*x + y*y + z*z);
                //step 6
                complex_double** h = (complex_double**)malloc((P+2)*sizeof(complex_double*)); //P+2 because derivatives
                complex_double** hx = (complex_double**)malloc((P+1)*sizeof(complex_double*));
                complex_double** hy = (complex_double**)malloc((P+1)*sizeof(complex_double*));
                complex_double** hz = (complex_double**)malloc((P+1)*sizeof(complex_double*));
                h[P+1] = (complex_double*)malloc((2*(P+1)+1) * sizeof(complex_double));
                for (int n = 0 ; n < P + 1 ; ++n) {
                    h[n] = (complex_double*)malloc((2*n+1) * sizeof(complex_double));
                    hx[n] = (complex_double*)malloc((2*n+1) * sizeof(complex_double));
                    hy[n] = (complex_double*)malloc((2*n+1) * sizeof(complex_double));
                    hz[n] = (complex_double*)malloc((2*n+1) * sizeof(complex_double));
                    for (int m = -n ; m < n + 1 ; ++m) {
                        h[n][m+n] = harmonic(m, n, x, y, z, r, fact);
                        h[n][m+n] = multiply_real(multiply_complex(h[n][m+n], b->local[n][m+n]), power(r, n+1));
                        double alpha1 = sqrt(n-m+1);
                        double alpha2 = sqrt(n+m+1);
                        double alpha3 = sqrt(n+m);
                        double alpha4 = sqrt(n-m);
                        hz[n][m+n] = multiply_real(h[n][m+n], alpha1 * alpha2 / r);
                        complex_double D_plus = zero();
                        complex_double D_minus = zero();
                        if (m+1+n <= 2*n) D_plus = multiply_real(h[n][m+1+n], alpha1 * alpha4 / (2*r));
                        if (m-1+n >= 0) D_minus = multiply_real(h[n][m-1+n], -alpha2 * alpha3 / (2*r));
                        hx[n][m+n] = add_complex(D_plus, D_minus);
                        hy[n][m+n] = multiply_complex(complex_1(), add_complex(D_minus, multiply_real(D_plus, -1)));
                        part->ax -= G*re(multiply_complex(hx[n][m+n], b->local[n][n+m]));
                        part->ay -= G*re(multiply_complex(hy[n][m+n], b->local[n][n+m]));
                        part->az -= G*re(multiply_complex(hz[n][m+n], b->local[n][n+m]));
                    }
                }
                //step 7
                for (int j = 0 ; j < b->n_L1 ; ++j) {
                    box* c = &o->elements[b->L1[j]];
                    for (int k = 0 ; k < c->n_particles ; ++k) {
                        particle* other = world + c->first_particle + k;
                        double dx = other->x - part->x;
                        double dy = other->y - part->y;
                        double dz = other->z - part->z;

                        double r2 = dx*dx + dy*dy + dz*dz + CUSHION*CUSHION;
                        double invr = 1.0/sqrt(r2);
                        double invr3 = invr*invr*invr;

                        part->ax += G*other->m * dx * invr3;
                        part->ay += G*other->m * dy * invr3;
                        part->az += G*other->m * dz * invr3;
                    }
                }
                //step 8
                for (int j = 0 ; j < b->n_L3 ; ++j) {
                    box* c = &o->elements[b->L3[j]];
                    if (c->n_particles <= P*P) {
                        for (int k = 0 ; k < c->n_particles ; ++k) {
                            particle* other = world + c->first_particle + k;
                            double dx = other->x - part->x;
                            double dy = other->y - part->y;
                            double dz = other->z - part->z;

                            double r2 = dx*dx + dy*dy + dz*dz + CUSHION*CUSHION;
                            double invr = 1.0/sqrt(r2);
                            double invr3 = invr*invr*invr;

                            part->ax += G*other->m * dx * invr3;
                            part->ay += G*other->m * dy * invr3;
                            part->az += G*other->m * dz * invr3;
                        }
                    }
                    else {
                        for (int n = 0 ; n < P + 1 ; ++n) {
                            for (int m = -n ; m < n + 1 ; ++m) {
                                part->ax -= G*re(multiply_complex(hx[n][m+n], c->multipole[n][n+m]));
                                part->ay -= G*re(multiply_complex(hy[n][m+n], c->multipole[n][n+m]));
                                part->az -= G*re(multiply_complex(hz[n][m+n], c->multipole[n][n+m]));
                            }
                        }
                    }
                }
                for (int n = 0 ; n < P + 1 ; ++n) {
                    free(h[n]);
                    free(hx[n]);
                    free(hy[n]);
                    free(hz[n]);
                }
                free(h[P+1]);
                free(h);
                free(hx);
                free(hy);
                free(hz);
            }
        }
    }
}

void fmm(data* d) {
    step_0(d->o);
    postorder(d->o, d, 0, upward_pass);
    preorder(d->o, d, 0, downward_pass);
}

void update_positions(data* d, int N, double dt) {
    for (int i = 0 ; i < N ; ++i) {
        particle* part = &d->w[i];
        part->x += (part->vx + part->ax*dt/2)*dt;
        part->vx += (part->ax + part->ax2)*dt/2;
        part->ax2 = part->ax;

        part->y += (part->vy + part->ay*dt/2)*dt;
        part->vy += (part->ay + part->ay2)*dt/2;
        part->ay2 = part->ay;

        part->z += (part->vz + part->az*dt/2)*dt;
        part->vz += (part->az + part->az2)*dt/2;
        part->az2 = part->az;
    }
    return;
}

void reset_data(data* d, int N, double simulation_size) {
    free_octree(d->o);
    d->o = init_octree(d->w, N, simulation_size);
    for (int i = 0 ; i < N ; ++i) {
        d->w[i].ax = 0;
        d->w[i].ay = 0;
        d->w[i].az = 0;
    }
    return;
}

void write_data(data* d, int N, FILE* file) {
    if (N == 2) {
        fprintf(file, 
        ",%.15lf,%.15lf,,%.15lf,%.15lf\n", 
        d->w[0].x, d->w[0].y, d->w[1].x, d->w[1].y
        );
    }
    else if (N == 9) {
        for (int i = 0 ; i < N ; ++i) {
            if (d->w[i].m == 0.0000029995) {
                fprintf(file, 
                    ",%.15lf,%.15lf,%.15lf\n", 
                    d->w[i].x, d->w[i].y, d->w[i].z
                    );
            }
        }
        
    }
    return;
}


