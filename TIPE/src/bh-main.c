/*
Notes : 
Currently implemented code, simulating the solar system, earth goes off to infinity :(
Maybe an integration problem because theta doesnt change the result

tests :
unless specified theta = 0.5
dt = 1/simulation_time : 1 day
we're looking at earth x 
horizons :       0.1615993742745284
dt = 1 day :     0.161620762399848
                 0.161600821899350 // after change
    theta = 0 :  0.161620762384551
dt = 0.1 day :   0.161620762407485
dt = 0.01 day :  0.161620762407561
                 0.161600036146096 // after change
dt = 0.00001 :   0.161600036145309 // after change

dt = 0.001 day : 0.161620762407562
    theta = 0    0.161620762392231
Conclusion : 
all go over and the more there are the more they go over
So integration isnt the problem

I tried epsilon = 0 and got NaN so I found the problem lol : self force calculation
self force calculation is a problrm because of insert_body n->h < MIN_H this needs to be rechecked
Small dt to see where my bodies are overlapping and if thats not the problem then redo insert_body riiip

Next steps:
add force calculation with h<H_MIN

compare to N² dumb simulation
energy conservation
sun earth simulation that can be compared to real calculations
dt convergence
*/

#include "barnes-hut.h"
#include <stdlib.h>


#define simulation_size 20 //au
#define time_step 1 //day
#define simulation_time 1 //time_steps

FILE** init_write(int N) {
    FILE** write = malloc(N * sizeof(FILE*));
    // in order Sun Mercury Venus Earth Mars Jupiter Saturn Uranus Neptune
    write[0] = fopen("bh_sun.txt", "w");
    fprintf(write[0], "sun,x,y,z,Vx,Vy,Vz\n");

    write[1] = fopen("bh_mercury.txt", "w");
    fprintf(write[1], "mercury,x,y,z,Vx,Vy,Vz\n");

    write[2] = fopen("bh_venus.txt", "w");
    fprintf(write[2], "venus,x,y,z,Vx,Vy,Vz\n");

    write[3] = fopen("bh_earth.txt", "w");
    fprintf(write[3], "earth,x,y,z,Vx,Vy,Vz\n");

    write[4] = fopen("bh_mars.txt", "w");
    fprintf(write[4], "mars,x,y,z,Vx,Vy,Vz\n");

    write[5] = fopen("bh_jupiter.txt", "w");
    fprintf(write[5], "jupiter,x,y,z,Vx,Vy,Vz\n");

    write[6] = fopen("bh_saturn.txt", "w");
    fprintf(write[6], "saturn,x,y,z,Vx,Vy,Vz\n");

    write[7] = fopen("bh_uranus.txt", "w");
    fprintf(write[7], "uranus,x,y,z,Vx,Vy,Vz\n");

    write[8] = fopen("bh_neptune.txt", "w");
    fprintf(write[8], "neptune,x,y,z,Vx,Vy,Vz\n");

    return write;
}

void close_write(FILE** write) {
    for (int i = 0 ; i < 9 ; i++) {
        fclose(write[i]);
    }
}

int main() {
    int N = 9;
    simulation* s = create_system(N);
    (void) init_system(s);
    octree* o = create_octree(N, simulation_size);

    FILE** write = init_write(N);

    // init accelerations
    for (int i = 0; i < N ; i++) {
        insert_body(o, s->bodies, 0, i);
    }
    for (int i = 0 ; i < s->N ; i++) {
        body b = s->bodies[i];
        vector f = (vector){0.0, 0.0, 0.0};
        (void) calculate_force(i, &f, o, s->bodies, 0);
        s->a[i].x = f.x / b.mass;
        s->a[i].y = f.y / b.mass;
        s->a[i].z = f.z / b.mass;
    }
    (void) write_data(s, write);
    for (int k = 0 ; k < simulation_time ; k++) {
        (void) leapfrog_step(s, o, time_step);
        (void) write_data(s, write);
   }
   (void) close_write(write);
   free_system(s);
   free_octree(o);

}