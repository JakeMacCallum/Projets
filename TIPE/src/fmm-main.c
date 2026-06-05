#include "fmm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {

//cd TIPE/src/ && gcc -Wall -Wextra -Wshadow -Wpedantic -fsanitize=address -fsanitize=address,undefined -g -O0 my_complex.c fmm.c fmm-main.c -lm && ./a.out && cd ../../

    srand(time(NULL));


    int N = 9;
    double simulation_size = 1000;
    int simulation_length = 100 * 10;
    double dt = 0.01;
    FILE* file;

    file = fopen("../data/data9.csv", "w");
    if (N==2) {
        file = fopen("../data/data2.csv", "w");
        fprintf(file, "Earth,x1,y1,Sun,x2,y2\n");
    }
    else if (N == 9) {
        file = fopen("../data/data9.csv", "w");
        fprintf(file, "Earth,x,y,z\n");
    }

    data* d = init_data(N, simulation_size);
    (void)fmm(d);

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
        fmm(d);

        for (int i = 0; i < N; ++i) {
            particle* p = &d->w[i];

            p->vx += 0.5 * dt * p->ax;
            p->vy += 0.5 * dt * p->ay;
            p->vz += 0.5 * dt * p->az;
        }

        write_data(d, N, file);
}
    /*
    for (int t = 0 ; t <= simulation_length ; ++t) {
        (void) write_data(d, N, file);
        (void) fmm(d);
        (void) update_positions(d, N, dt);
        if (t < simulation_length) (void) reset_data(d, N, simulation_size);
    }
    */

    free_data(d);
    return 0;
}
