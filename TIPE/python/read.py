import pygame
import pandas as pd
import numpy as np

TIME = 10000

# define colours
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

# system
SCALE = 100

# pygame
pygame.init()

size = width, height = 900, 900
screen = pygame.display.set_mode((width, height))
clock = pygame.time.Clock()
fps = 60

df = pd.read_csv('TIPE/data/data2.csv', names = ['Earth','x1','y1', 'Sun', 'x2', 'y2'])

def make_tuple(x, y):
    return tuple(int(t) for t in np.array([float(x), float(y)]) * SCALE + np.array([screen.get_width() / 2, screen.get_height() / 2]))

data_earth = list(map(make_tuple, (df.x1.tolist())[1:TIME], (df.y1.tolist())[1:TIME]))
data_sun = list(map(make_tuple, (df.x2.tolist())[1:TIME], (df.y2.tolist())[1:TIME]))

#pygame.draw.circle(screen, body.colour, body.get_position(screen, self.scale), body.radius)


running = True
i = 0

while running:

    screen.fill((0, 0, 0))
    pygame.draw.circle(screen, GREEN, data_earth[i], 10)
    pygame.draw.circle(screen, RED, data_sun[i], 10)
    pygame.display.flip()
    if i >= len(data_sun):
        i = 0
    else:
        print(i)
        i+=1


    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    clock.tick(fps)

pygame.quit()

