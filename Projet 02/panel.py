import pygame
import json

with open('pokemon_list.json', 'r') as f:
    pokemon_list = json.load(f)


class Panel:
    def __init__(self, screen, width, height, image_separation):
        self.screen = screen
        self.width = width
        self.height = height
        self.image_separation = image_separation
        self.img_size = width // 20, width // 20
        self.img_width, self.img_height = self.img_size
        self.panel_number = 0
        n = 0
        for i in range(18):
            length = i * self.img_width + (i + 1) * self.image_separation
            if length < self.width:
                n = i - 2
        self.number_of_pokemon = n
        self.x = width // 2 - (n / 2 * self.img_width + (n / 2 - 1) * self.image_separation + image_separation // 2)
        self.y = height // 1.5
        self.separator = self.x - (self.img_width + self.image_separation)
        self.pokemon_list = []
        temp_list = []
        count = 0
        for pokemon in pokemon_list:
            temp_list.append(pokemon)
            count += 1
            if count % (n * 2) == 0:
                self.pokemon_list.append(temp_list)
                temp_list = []

    def draw(self):
        Dict = {}
        n = self.number_of_pokemon
        x = self.x
        y = self.y
        for names in self.pokemon_list[self.panel_number]:
            n -= 1
            pokemon_img = pygame.image.load(f'img/pokemon/front/{names}.png').convert_alpha()
            pokemon_img = pygame.transform.scale(pokemon_img, (self.img_width, self.img_height))
            pokemon_rect = pokemon_img.get_rect(topleft=(x, y))
            Dict[names] = (pokemon_img, pokemon_rect)
            x += self.img_width + self.image_separation
            if n == 0:
                n = self.number_of_pokemon
                x = self.x
                y = self.y - (self.image_separation - self.img_width)
        return Dict

    def next(self):
        self.panel_number += 1
        return self.draw()

    def previous(self):
        self.panel_number -= 1
        return self.draw()
