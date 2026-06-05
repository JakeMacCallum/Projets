import pygame
from body import *
from system import *

# define colours
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

# system
SCALE = 1 / 1_000_000
time_speed = 50_000
system = System(
    [
        Body("Earth", 10, 6378, np.array([1.521e8, 0.0]), np.array([0.0, 5.0]), BLUE),
        Body("Sun", 20, 695700, np.array([0.0, 0.0]), np.array([0.0, 0.0]), RED),
        Body("Intrukgraich", 10, 8000, np.array([-1.5e8, 0.0]), np.array([0.0, -5.0]), GREEN),
        Body("7-pmiair", 10, 7000, np.array([-1.5e8, -1.5e8]), np.array([3.0, -2.0]), WHITE)
    ],
    time_speed,
    SCALE,
    True
)

# pygame
pygame.init()

size = width, height = 900, 900
screen = pygame.display.set_mode((width, height))
clock = pygame.time.Clock()
fps = 60

running = True

while running:
    screen.fill((0, 0, 0))
    
    system.euler_update_positions()
    system.print_system(screen)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    clock.tick(fps)

pygame.quit()

