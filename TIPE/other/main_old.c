#include "system.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <json-c/json.h>

int main(int argc, char *argv[]) 
{
    int n = 2;
    body* bodies = malloc(n * sizeof(body));
    bodies[0] = init_body("Earth", 6378, 2, (vector){152100000, 0}, (vector){0, 5}, (vector){0, 0});
    bodies[1] = init_body("Sun", 695700, 1, (vector){0, 0}, (vector){0, 0}, (vector){0, 0});

    systems* s = init_system(n, 100000, true, bodies);

    FILE* fptr = fopen("data.csv", "w");
    fprintf(fptr, "Earth,x1,y1,Sun,x2,y2\n");
    int time = 10000;  // ,152100000,0,,0,0
    for (int i = 0 ; i < time ; i++) {
        fprintf(fptr, ",%f,%f,,%f,%f\n", bodies[0].position.x, bodies[0].position.y, bodies[1].position.x, bodies[1].position.y);
        euler_update_positions(s);
    }
    
    fclose(fptr);
    free_systems(s);
    return 0;
}

