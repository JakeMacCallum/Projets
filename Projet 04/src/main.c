#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "minesweeper.h"



int main(int argc, char *argv[])
{
	int **game_board = create_table(N_ROWS, N_COLUMNS);
	int **visible_board = create_table(N_ROWS, N_COLUMNS);
	int **bomb_coords = create_table(N_BOMBS, 2);


	int game_state = PLAYING;

	place_bombs(game_board, bomb_coords);
	update_numbers(game_board, bomb_coords);

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	//Lancement SDL
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
		SDL_ExitWithError("Initialisation SDL");

	//Création fenêtre + rendu
	if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer) != 0)
		SDL_ExitWithError("Creation fenetre et/ou rendu echouee");
	
	/*--------------------------------------------------------------------------------*/
	
	unsigned int frame_limit = 0;
	frame_limit = SDL_GetTicks() + FRAME_DELAY;


	SDL_Rect tile[N_ROWS][N_COLUMNS];
	for(int i = 0 ; i < N_ROWS ; i++)
		for(int j = 0 ; j < N_COLUMNS ; j++)
		{
			tile[i][j].x = 100 + j * TILE_SIZE;
			tile[i][j].y = 100 + i * TILE_SIZE;
			tile[i][j].w = TILE_SIZE ;
			tile[i][j].h = TILE_SIZE;
		}

	
	SDL_Texture *textures[N_TEXTURES] = {NULL};

	load_textures(window, renderer, textures);

	SDL_bool program_launched = SDL_TRUE;

	while (program_launched)
	{
		update_screen(window, renderer, visible_board, tile, game_state, textures);

		SDL_RenderPresent(renderer);

		limit_fps(frame_limit);
		frame_limit = SDL_GetTicks() + FRAME_DELAY;

		SDL_Event event;

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_MOUSEBUTTONDOWN:
					SDL_Point click_point = {event.button.x, event.button.y};
					int click_rect[2] = {0, 0};
					if(get_click_rect(&click_point, &tile[0][0], click_rect) != 0)
					{
						printf("%d\n", game_board[click_rect[0]][click_rect[1]]);
						switch (event.button.button)
						{
							case SDL_BUTTON_LEFT:
								switch (visible_board[click_rect[0]][click_rect[1]])
								{
								case HIDDEN:
									switch (game_board[click_rect[0]][click_rect[1]])
									{
									case BOMB:
										game_state = LOSE;
										program_launched = SDL_FALSE;
										break;
									default:
										show_area(visible_board, game_board, click_rect[0], click_rect[1]);
										printf("outofcall");
										break;
									}
									break;
								default:
									break;
								}
								break;
							case SDL_BUTTON_RIGHT:
								switch (visible_board[click_rect[0]][click_rect[1]])
									{
									case HIDDEN:
										visible_board[click_rect[0]][click_rect[1]] = FLAG;
										break;
									case FLAG:
										visible_board[click_rect[0]][click_rect[1]] = QUESTION;
										break;
									case QUESTION:
										visible_board[click_rect[0]][click_rect[1]] = HIDDEN;
										break;
									
									default:
										break;
									}
									break;
							default:
								break;
						}
					}								
					break;
				case SDL_QUIT:
					program_launched = SDL_FALSE;
					break;
				default:
					break;

			}
		}
	}
	
	SDL_DestroyRenderer(renderer);
	free_table(game_board);
	free_table(bomb_coords);
	free_table(visible_board);
	/*--------------------------------------------------------------------------------*/
	
	cleanup_textures(textures);
	SDL_Quit();

	return EXIT_SUCCESS;
}


/*

DRAW POINT LINE AND RECTANGLE

if(SDL_SetRenderDrawColor(renderer, 112, 126, 200, SDL_ALPHA_OPAQUE) != 0)
		SDL_ExitWithError("Changement couleur du rendu echouee");
	
if (SDL_RenderDrawPoint(renderer, 100, 450) != 0)
	SDL_ExitWithError("Impossible de dessiner le point");
if (SDL_RenderDrawLine(renderer, 50, 50, 500, 600) != 0)
	SDL_ExitWithError("Impossible de dessiner la ligne");

SDL_Rect rectangle;
rectangle.x = 300;
rectangle.y = 200;
rectangle.w = 200;
rectangle.h = 120;

if (SDL_RenderDrawRect(renderer, &rectangle) != 0)
	SDL_ExitWithError("Impossible de dessiner le rectangle");
SDL_RenderPresent(renderer);

*/

/*

SHOW IMAGE

SDL_Surface *image = NULL;
SDL_Texture *texture = NULL;

image = SDL_LoadBMP("src/sample_640×426.bmp");

if(image == NULL)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_ExitWithError("Impossible de charger l'image");
}

texture = SDL_CreateTextureFromSurface(renderer, image);
SDL_FreeSurface(image);

if(texture == NULL)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_ExitWithError("Impossible de creer la texture");
}

SDL_Rect rectangle;

if(SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h) != 0)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_ExitWithError("Impossible de charger la texture");
}

rectangle.x = (WINDOW_WIDTH - rectangle.w) / 2;
rectangle.y = (WINDOW_HEIGHT - rectangle.h) / 2;

if(SDL_RenderCopy(renderer, texture, NULL, &rectangle) != 0)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_ExitWithError("Impossible d'afficher la texture");
}

SDL_RenderPresent(renderer);
*/