#include "fmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define G 0.00029591220828559093

void direct_sum(data* d, int N)
{
    for (int i = 0; i < N; ++i) {
        d->w[i].ax = 0.0;
        d->w[i].ay = 0.0;
        d->w[i].az = 0.0;
    }

    for (int i = 0; i < N; ++i) {
        particle* a = &d->w[i];

        for (int j = i + 1; j < N; ++j) {
            particle* b = &d->w[j];

            double dx = b->x - a->x;
            double dy = b->y - a->y;
            double dz = b->z - a->z;

            double r2 = dx*dx + dy*dy + dz*dz;
            double invr = 1.0 / sqrt(r2);
            double invr3 = invr * invr * invr;

            double fax = G * dx * invr3;
            double fay = G * dy * invr3;
            double faz = G * dz * invr3;

            a->ax += b->m * fax;
            a->ay += b->m * fay;
            a->az += b->m * faz;

            b->ax -= a->m * fax;
            b->ay -= a->m * fay;
            b->az -= a->m * faz;
        }
    }
}

int main() {

//cd TIPE/src/ && gcc -Wall -Wextra -Wshadow -Wpedantic -fsanitize=address -fsanitize=address,undefined -g -O0 my_complex.c fmm.c fmm-main.c -lm && ./a.out && cd ../../

    srand(time(NULL));


    int N = 9;
    double simulation_size = 1000;
    int simulation_length = 100*365;
    double dt = 0.01;
    FILE* file;

    file = fopen("../data/data9_direct.csv", "w");

    data* d = init_data(N, simulation_size);
    direct_sum(d, N);

    for (int t = 0; t < simulation_length; ++t) {

        for (int i = 0; i < N; ++i) {
            particle* p = &d->w[i];

            p->vx += 0.5 * dt * p->ax;
            p->vy += 0.5 * dt * p->ay;
            p->vz += 0.5 * dt * p->az;
        }

        for (int i = 0; i < N; ++i) {
            particle* p = &d->w[i];

            p->x += dt * p->vx;
            p->y += dt * p->vy;
            p->z += dt * p->vz;
        }

        reset_data(d, N, simulation_size);
        direct_sum(d, N);

        for (int i = 0; i < N; ++i) {
            particle* p = &d->w[i];

            p->vx += 0.5 * dt * p->ax;
            p->vy += 0.5 * dt * p->ay;
            p->vz += 0.5 * dt * p->az;
        }

        write_data(d, N, file);
}
    

    free_data(d);
    return 0;
}
