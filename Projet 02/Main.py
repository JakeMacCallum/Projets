import pygame
import random
import pokemon
import panel
import healthbar

pygame.init()

clock = pygame.time.Clock()
fps = 60

# game window
size = width, height = 960, 540
screen = pygame.display.set_mode((width, height))

# define font
font1 = pygame.font.Font("fonts/Retro Gaming.ttf", 30)
font2 = pygame.font.Font("fonts/Retro Gaming.ttf", 60)
font3 = pygame.font.Font("fonts/Retro Gaming.ttf", 20)
# define colours
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (186, 6, 4)
PINK = (255, 105, 106)
YELLOW = (255, 190, 78)

# variables
clicked = False
image_separation = 8
game_stage = 0
pokemon_button_dict = {}
player_pokemon = None
player = None
opponent = None
opponent_pokemon = None
click_cooldown = 0
game_over = 0
attack1 = None
attack2 = None
attack3 = None
attack4 = None
rect1 = None
rect2 = None
rect3 = None
rect4 = None
rect_width = 1
current_fighter = 1
action_cooldown = 0
action_wait_time = 120
text = [""]
player_healthbar = None
opponent_healthbar = None

# create panel
panel = panel.Panel(screen, width, height, image_separation)

# text
play_txt = font2.render("Play", True, PINK)
choose_txt = font1.render("Choose your pokemon", True, WHITE)
win_txt = font2.render("You Win!", True, YELLOW)
lose_txt = font2.render("Game Over", True, YELLOW)
restart_txt = font2.render("Restart ?", True, YELLOW)

# load images
red_img = pygame.image.load('img/Background/background.png').convert_alpha()
red_img = pygame.transform.scale(red_img, size)

space_img = pygame.image.load('img/Background/battle_background.png').convert_alpha()
space_img = pygame.transform.scale(space_img, size)

txt_box_img = pygame.image.load('img/UI/txt_box.png').convert_alpha()
txt_box_img = pygame.transform.scale(txt_box_img, (txt_box_img.get_width() // 2, txt_box_img.get_height() // 2))

next_img = pygame.image.load('img/UI/Next.png').convert_alpha()
next_img = pygame.transform.scale(next_img, (panel.img_width, panel.img_height))

previous_img = pygame.image.load('img/UI/Previous.png').convert_alpha()
previous_img = pygame.transform.scale(previous_img, (panel.img_width, panel.img_height))

healthbar_img = pygame.image.load('img/UI/healthbar.png').convert_alpha()
healthbar_img = pygame.transform.scale(healthbar_img, (width//5, 30))

background_img = red_img

# create rects
play_rect = play_txt.get_rect(center=(width // 2, height // 1.5))
choose_rect = choose_txt.get_rect(center=(width // 2, height // 8))
win_rect = win_txt.get_rect(center=(width // 2, height // 4))
lose_rect = lose_txt.get_rect(center=(width // 2, height // 4))
restart_rect = restart_txt.get_rect(center=(width // 2, height // 1.5))
txt_box_rect = txt_box_img.get_rect(center=(width // 1.3, height // 1.3))

next_rect = next_img.get_rect(topleft=(panel.width - panel.x, panel.y))
previous_rect = previous_img.get_rect(topleft=(panel.separator, panel.y))
healthbar_1_rect = healthbar_img.get_rect(topleft=(width // 4, height // 4 + 2 * width // 10))
healthbar_2_rect = healthbar_img.get_rect(topleft=(width // 1.5, height // 7 + width // 10))

running = True

while running:

    clock.tick(fps)

    screen.blit(background_img, (0, 0))

    # main menu
    if game_stage == 0:
        screen.blit(play_txt, play_rect)
        pygame.draw.rect(screen, RED, play_rect, 1)

    # character selection
    elif game_stage == 1:
        screen.blit(choose_txt, choose_rect)
        screen.blit(previous_img, previous_rect)
        screen.blit(next_img, next_rect)
        for pokemons in pokemon_button_dict.keys():
            screen.blit(pokemon_button_dict[pokemons][0], pokemon_button_dict[pokemons][1])

    # game
    elif game_stage == 2:
        if game_over == 0:
            screen.blit(txt_box_img, txt_box_rect)
            screen.blit(healthbar_img, healthbar_1_rect)
            screen.blit(healthbar_img, healthbar_2_rect)
            player_healthbar.draw(screen , player.stats['hp'])
            opponent_healthbar.draw(screen, opponent.stats['hp'])
            for i in range(len(text)):
                txt_box_txt = font3.render(text[i], True, BLACK)
                txt_rect = txt_box_txt.get_rect(center=(width // 1.3, height // 1.4 + font3.get_height() + 15 * i))
                screen.blit(txt_box_txt, txt_rect)

            screen.blit(
                pygame.transform.scale(pygame.image.load(f'img/pokemon/back/{player_pokemon}.png').convert_alpha(),
                                       (2 * width // 10, 2 * width // 10)), (width // 4, height // 4))
            screen.blit(
                pygame.transform.scale(pygame.image.load(f'img/pokemon/front/{opponent_pokemon}.png').convert_alpha(),
                                       (width // 10, width // 10)), (width // 1.5, height // 7))
            if player.stats['hp'] > 0:
                if current_fighter == 1:
                    # player turn
                    screen.blit(attack1, rect1)
                    pygame.draw.rect(screen, WHITE, rect1, rect_width)
                    screen.blit(attack2, rect2)
                    pygame.draw.rect(screen, WHITE, rect2, rect_width)
                    screen.blit(attack3, rect3)
                    pygame.draw.rect(screen, WHITE, rect3, rect_width)
                    screen.blit(attack4, rect4)
                    pygame.draw.rect(screen, WHITE, rect4, rect_width)
            else:
                game_over = -1
            if opponent.stats['hp'] > 0:
                if current_fighter == 2:
                    action_cooldown += 1
                    if action_cooldown >= action_wait_time:
                        # computer turn
                        text = opponent.evaluate(player)
                        current_fighter = 1
                        action_cooldown = 0
            else:
                game_over = 1
        elif game_over != 0:
            background_img = red_img
            screen.blit(restart_txt, restart_rect)
            pygame.draw.rect(screen, RED, restart_rect, 1)
            if game_over == 1:
                # player win
                screen.blit(win_txt, win_rect)
            else:
                # player lose
                screen.blit(lose_txt, lose_rect)
    # event check
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.MOUSEBUTTONDOWN:
            clicked = True
            # main menu
            if game_stage == 0:
                if play_rect.collidepoint(event.pos):
                    game_stage = 1
                    pokemon_button_dict = panel.draw()
            # character selection
            elif game_stage == 1:
                for pokemons in pokemon_button_dict.keys():
                    if pokemon_button_dict[pokemons][1].collidepoint(event.pos):
                        game_stage = 2
                        background_img = space_img
                        player_pokemon = pokemons
                        opponent_pokemon = random.choice(list(pokemon_button_dict.keys()))
                        player = pokemon.Pokemon(player_pokemon)
                        opponent = pokemon.Pokemon(opponent_pokemon)
                        player_healthbar = healthbar.Healthbar(width // 4, height // 4 + 2 * width // 10, player.stats['hp'])
                        opponent_healthbar = healthbar.Healthbar(width // 1.5, height // 7 + width // 10, opponent.stats['hp'])
                        # make attacks
                        attack1 = font1.render(list(player.moves)[0], True, WHITE)
                        attack2 = font1.render(list(player.moves)[1], True, WHITE)
                        attack3 = font1.render(list(player.moves)[2], True, WHITE)
                        attack4 = font1.render(list(player.moves)[3], True, WHITE)
                        rect1 = attack1.get_rect(midright=(width // 4 - 15, height // 1.4))
                        rect2 = attack2.get_rect(midright=(width // 4 - 15, height // 1.25))
                        rect3 = attack1.get_rect(midleft=(width // 4 + 15, height // 1.4))
                        rect4 = attack2.get_rect(midleft=(width // 4 + 15, height // 1.25))

                if panel.panel_number == 0:
                    if previous_rect.collidepoint(event.pos):
                        pokemon_button_dict = panel.previous()
                if panel.panel_number < len(panel.pokemon_list) - 1:
                    if next_rect.collidepoint(event.pos):
                        pokemon_button_dict = panel.next()
            # game
            elif game_stage == 2:
                if rect1.collidepoint(event.pos) and current_fighter == 1:
                    text = player.attack(list(player.moves)[0], opponent)
                    current_fighter = 2
                if rect2.collidepoint(event.pos) and current_fighter == 1:
                    text = player.attack(list(player.moves)[1], opponent)
                    current_fighter = 2
                if rect3.collidepoint(event.pos) and current_fighter == 1:
                    text = player.attack(list(player.moves)[2], opponent)
                    current_fighter = 2
                if rect4.collidepoint(event.pos) and current_fighter == 1:
                    text = player.attack(list(player.moves)[3], opponent)
                    current_fighter = 2
                # restart
                if game_over != 0:
                    if restart_rect.collidepoint(event.pos):
                        player_pokemon = None
                        opponent_pokemon = None
                        game_stage = 1
                        game_over = 0
                        text = [""]
                        current_fighter = 1
        else:
            clicked = False

    pygame.display.update()
pygame.quit()
