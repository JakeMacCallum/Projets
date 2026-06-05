import pygame


class Healthbar:
    def __init__(self, x, y, hp):
        self.x = x
        self.y = y
        self.hp = self.max_hp = hp

    def draw(self, screen, hp):
        # update with new health
        self.hp = hp
        # calculate health ratio
        ratio = self.hp / self.max_hp
        pygame.draw.rect(screen, (255, 0, 0),
                         (self.x + screen.get_width() // 16.27, self.y + screen.get_height() // 41.5,
                          screen.get_width() // 8.5, screen.get_height() // 108))
        pygame.draw.rect(screen, (0, 255, 0),
                         (self.x + screen.get_width() // 16.27, self.y + screen.get_height() // 41.5,
                          (screen.get_width() // 8.5) * ratio, screen.get_height() // 108))

