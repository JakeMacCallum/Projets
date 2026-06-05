import numpy as np

G = 15000
class Body:
    def __init__(self, name, radius, mass, position, velocity, colour):
        self.name = name
        self.mass = mass
        self.radius = radius
        self.position = position
        self.velocity = velocity
        self.forces = np.array([0.0, 0.0])
        self.colour = colour
    

    def get_position(self, screen, scale):
        return tuple(int(x) for x in self.position * scale + np.array([screen.get_width() // 2, screen.get_height() // 2]))


def update_forces(body1, body2, gravitation):
    if gravitation:
        update_gravitation(body1, body2)
    print(body1.forces)
    print(body2.forces)


def update_gravitation(body1, body2):
    r_vec = body2.position - body1.position
    dist = distance(body1, body2)
    if dist == 0:
        dist = 1
    direction = r_vec / dist  # unit vector
    force_magnitude =  G * body1.mass * body2.mass / (dist ** 2)
    body1.forces += direction * force_magnitude
    body2.forces += -body1.forces  # equal and opposite


def distance(body1, body2):
    return np.linalg.norm(body2.position - body1.position)