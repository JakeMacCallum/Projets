#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define G 15000.0
#define SCALE 1.0 / 100000000.0

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


body init_body(char* name, double mass, double radius, vector position, vector velocity, vector forces)
{
    return (body){name, mass, radius, position, velocity, forces};
}


double distance(body* b1, body* b2) 
{
    double dx = b1->position.x - b2->position.x;
    double dy = b1->position.y - b2->position.y;
    return sqrt(dx * dx + dy * dy);
}

void update_gravitation(body* b1, body* b2)
{
    vector r_vec = {b2->position.x - b1->position.x, b2->position.y - b1->position.y};
    double dist = distance(b1, b2);
    
    dist = dist < 1.0 ? 1.0 : dist;
    
    vector direction = {r_vec.x / dist, r_vec.y / dist};
    
    double force_magnitude = G * b1->mass * b2->mass / (dist * dist);
    
    b1->forces.x += direction.x * force_magnitude;
    b1->forces.y += direction.y * force_magnitude;
    b2->forces.x -= direction.x * force_magnitude;
    b2->forces.y -= direction.y * force_magnitude;
}

void update_body_forces(body* b1, body* b2, bool gravitation) 
{
    if (gravitation) update_gravitation(b1, b2);
    return;
}




/*
import body
import pygame

class System:
    
    def update_forces(self):
        for b in self.bodies:
            b.forces = 0
        for i in range(len(self.bodies)):
            for j in range(i+1, len(self.bodies)):
                body.update_forces(self.bodies[i], self.bodies[j], self.gravitation)
        


    def euler_update_positions(self):
        for body in self.bodies:
            body.velocity += (body.forces / body.mass) * self.dt
            body.position += body.velocity * self.dt
        self.update_forces()
        

    def verlet_update_positions(self):
        old_accelerations = []
        # update positions
        for body in self.bodies:
            acceleration = body.forces / body.mass
            body.position += (body.velocity + 0.5 * acceleration * self.dt) * self.dt
            old_accelerations.append(acceleration)

        self.update_forces()

        #update velocities
        for i in range(len(self.bodies)):
            body = self.bodies[i]
            body.velocity += 0.5 * (old_accelerations[i] + body.forces / body.mass) * self.dt


    def print_system(self, screen):
        for body in self.bodies:
            pygame.draw.circle(screen, body.colour, body.get_position(screen, self.scale), body.radius)
        pygame.display.flip()
        

*/



systems* init_system(int n, double dt, bool g, body* bodies)
{
    systems* s = malloc(sizeof(systems));
    s->bodies = bodies;
    s->size = n;
    s->dt = dt;
    s->gravitation = g;
    return s;
}


void free_systems(systems* s) 
{
    free(s->bodies);
    free(s);
}

void update_forces(systems* s)
{
    for (int i = 0 ; i < s->size ; i++) s->bodies[i].forces = (vector){0, 0};
    for (int i = 0 ; i < s->size ; i++) {
        // printf("x : %f\t\ty : %f\n", s->bodies[i].position.x, s->bodies[i].position.y);
        for (int j = i + 1 ; i < s->size ; i++) {
            update_body_forces(&s->bodies[i], &s->bodies[j], s->gravitation);
        }
    }
    return;
}

void euler_update_positions(systems* s)
{
    for (int i = 0 ; i < s->size ; i++) {
        s->bodies[i].velocity.x += (s->bodies[i].forces.x / s->bodies[i].mass) * s->dt;
        s->bodies[i].velocity.y += (s->bodies[i].forces.y / s->bodies[i].mass) * s->dt;
        s->bodies[i].position.x += s->bodies[i].velocity.x * s->dt;
        s->bodies[i].position.y += s->bodies[i].velocity.y * s->dt;
    }
    update_forces(s);
    return;
}


void verlet_update_positions(systems* s)
{
    vector* old_accelerations = malloc(s->size * sizeof(vector));
    for (int i = 0 ; i < s->size ; i++) {
        vector acceleration = {s->bodies[i].forces.x / s->bodies[i].mass, s->bodies[i].forces.y / s->bodies[i].mass};
        s->bodies[i].position.x += (s->bodies[i].velocity.x + 0.5 * acceleration.x * s->dt) * s->dt;
        s->bodies[i].position.y += (s->bodies[i].velocity.y + 0.5 * acceleration.y * s->dt) * s->dt;
        old_accelerations[i] = acceleration;
    }

    update_forces(s);

    for (int i = 0 ; i < s->size ; i++) {
        s->bodies[i].velocity.x += 0.5 * (old_accelerations[i].x + s->bodies[i].forces.x / s->bodies[i].mass) * s->dt;
        s->bodies[i].velocity.y += 0.5 * (old_accelerations[i].y + s->bodies[i].forces.y / s->bodies[i].mass) * s->dt;
    }
    return;
}
        