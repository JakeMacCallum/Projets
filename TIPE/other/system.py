import body
import pygame

class System:
    def __init__(self, bodies, dt, scale, gravitation):
        self.bodies = bodies
        self.dt = dt
        self.gravitation = gravitation
        self.scale = scale
    def add_body(self, new):
        self.bodies.append(new)

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
        
