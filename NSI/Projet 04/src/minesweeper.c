#include <stdio.h>
#include <SDL.h>
#include <time.h>
#include "minesweeper.h"

void SDL_ExitWithError(const char *message)
{
	SDL_Log("ERREUR : %s\n", message, SDL_GetError());
	SDL_Quit();
	exit(EXIT_FAILURE);
}

void SDL_EffacerRendu(SDL_Renderer *renderer)
{
	if(SDL_RenderClear(renderer) != 0)
		SDL_ExitWithError("Effacement rendu echouee");
}

SDL_Texture *SDL_Load(SDL_Window *window, SDL_Renderer *renderer, char* location)
{
	SDL_Surface *image = NULL;
	SDL_Texture *texture = NULL;

	image = SDL_LoadBMP(location);

	if(image == NULL)
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		printf("%s\n", location);
		SDL_ExitWithError("Impossible de charger l'image");
	}

	texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);

	if(texture == NULL)
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		printf("%s", location);
		SDL_ExitWithError("Impossible de creer la texture");
	}
	return texture;
}

void SDL_Show(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *destination)
{
	if(SDL_QueryTexture(texture, NULL, NULL, &(*destination).w, &(*destination).h) != 0)
		{
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_ExitWithError("Impossible de charger la texture");
		}
	if(SDL_RenderCopy(renderer, texture, NULL, destination) != 0)
		{
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_ExitWithError("Impossible d'afficher la texture");
		}
}
int **create_table(int rows, int collumns)
{
	int **table = malloc(rows * sizeof(int *));
	for(int i = 0 ; i < rows ; i++)
	{
		table[i] = malloc(collumns * sizeof(int *));
		for(int j = 0 ; j < collumns ; j++)
		{
			table[i][j] = HIDDEN;
		}
	}
	return table;
}

void free_table(int **table) 
{
    for (int i = 0; i < N_ROWS; i++) {
        free(table[i]);
    }
    free(table);
}

int get_click_rect(SDL_Point *point, SDL_Rect *tilemap, int *click_rect)
{
	for(int i = 0 ; i < N_ROWS ; i++)
		for(int j = 0 ; j < N_COLUMNS ; j++)
		{	
			if(SDL_PointInRect(point, tilemap + j + i * N_COLUMNS) == SDL_TRUE)
			{
				click_rect[0] = i;
				click_rect[1] = j;
				return 1;
			}				
		}
	return 0;
}


void update_screen(SDL_Window *window,
                   SDL_Renderer *renderer,
                   int **table, 
                   SDL_Rect (*tilemap)[N_COLUMNS],
                   int game_state,
                   SDL_Texture *textures[])
{
    switch (game_state)
    {
        case PLAYING:
            for (int i = 0; i < N_ROWS; i++)
				for (int j = 0; j < N_COLUMNS; j++)
                {
                    int texture_index = table[i][j];

					SDL_Show(window, renderer, textures[texture_index], &tilemap[i][j]);
                }
            break;
        case LOSE:
            break;
        case WIN:
            break;
    }
}


void shuffle(int *array, int n) 
{
    if (n > 1) 
	{
        for (int i = 0; i < n - 1; i++) 
		{
            int j = i + rand() / (RAND_MAX / (n - i) + 1);
            int temp = array[j];
            array[j] = array[i];
            array[i] = temp;
        }
    }
}

void place_bombs(int **minefield, int **bomb_coords) 
{
    srand(time(NULL));

    int total_cells = N_ROWS * N_COLUMNS;
    int *all_coords = malloc(total_cells * sizeof(int));
    for (int i = 0; i < total_cells; i++) 
        all_coords[i] = i;

    shuffle(all_coords, total_cells);

    for (int i = 0; i < N_BOMBS; i++) 
	{
        int index = all_coords[i];
        int x = index / N_COLUMNS;
        int y = index % N_COLUMNS;
        minefield[x][y] = BOMB;
        bomb_coords[i][0] = x;
        bomb_coords[i][1] = y;
    }

    free(all_coords);
}

void update_numbers(int **minefield, int **bomb_coords)
{
	for (int i = 0 ; i < N_BOMBS ; i++)
		{
			for (int x = -1; x < 2; x++) 
				for (int y = -1; y < 2; y++)
				{
					if(0 > bomb_coords[i][0] + x || bomb_coords[i][0] + x >= N_ROWS)
						continue;
					else if(0 > bomb_coords[i][1] + y || bomb_coords[i][1] + y >= N_COLUMNS)
						continue;
					else if(minefield[bomb_coords[i][0] + x][bomb_coords[i][1] + y] == BOMB)
						continue;
					else
						minefield[bomb_coords[i][0] + x][bomb_coords[i][1] + y] += 1;
				}
		}
}

void load_textures(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *textures[]) 
{
    textures[HIDDEN] = SDL_Load(window, renderer, "src/img/hidden.bmp");
	textures[NOTHING] = SDL_Load(window, renderer, "src/img/nothing.bmp");

	textures[ONE] = SDL_Load(window, renderer, "src/img/one.bmp");
	textures[TWO] = SDL_Load(window, renderer, "src/img/two.bmp");
	textures[THREE] = SDL_Load(window, renderer, "src/img/three.bmp");
	textures[FOUR] = SDL_Load(window, renderer, "src/img/four.bmp");
	textures[FIVE] = SDL_Load(window, renderer, "src/img/five.bmp");
	textures[SIX] = SDL_Load(window, renderer, "src/img/six.bmp");
	textures[SEVEN] = SDL_Load(window, renderer, "src/img/seven.bmp");
	textures[EIGHT] = SDL_Load(window, renderer, "src/img/eight.bmp");

	textures[BOMB] = SDL_Load(window, renderer, "src/img/bomb.bmp");
	textures[FLAG] = SDL_Load(window, renderer, "src/img/flag.bmp");
	textures[QUESTION] = SDL_Load(window, renderer, "src/img/question.bmp");
	
}

void cleanup_textures(SDL_Texture *textures[]) 
{
    for (int i = 0; i < N_TEXTURES; i++)
        if (textures[i] != NULL) 
		{
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
}

void limit_fps(unsigned int limit)
{
	unsigned int ticks = SDL_GetTicks();

	if(limit < ticks)
		return;
	else if(limit > ticks + FRAME_DELAY)
		SDL_Delay(FRAME_DELAY);
	else
		SDL_Delay(limit - ticks);
}

void show_area(int **visible_board, int **game_board, int x, int y)
{
	if(game_board[x][y] == HIDDEN)
		visible_board[x][y] = NOTHING;
	else
		visible_board[x][y] = game_board[x][y];
	printf("(%d, %d): %d\n", x, y, game_board[x][y]);
	if(game_board[x][y] == HIDDEN)
		for (int i = -1; i < 2; i++) 
			for (int j = -1; j < 2; j++)
			{
				if(0 > x + i || x + i >= N_ROWS)
						continue;
				else if(0 > y + j || y + j >= N_COLUMNS)
					continue;
				else if(i == 0 && j == 0)
					continue;
				else
					if(visible_board[x + i][y + j] == HIDDEN)
					{
						show_area(visible_board, game_board, x + i, y + j);
							printf("Recursive call at (%d, %d)\n", x + i, y + j);
					}
			}
	else
		return;	
}
