#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000

#define FRAME_DELAY 16

#define DEBUGGING &tilemap[i][j]

#define N_TEXTURES 13
#define PLAYING 0
#define LOSE -1
#define WIN 1

#define N_ROWS 50
#define N_COLUMNS 100

#define TILE_SIZE 16

#define N_BOMBS 750


#define HIDDEN 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define SIX 6
#define SEVEN 7
#define EIGHT 8
#define NOTHING 9
#define BOMB 10
#define FLAG 11
#define QUESTION 12

extern void SDL_ExitWithError(const char *message);
extern void SDL_EffacerRendu(SDL_Renderer *renderer);
extern SDL_Texture *SDL_Load(SDL_Window *window, SDL_Renderer *renderer, char* location);
extern void SDL_Show(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *destination);
extern int **create_table(int rows, int columns);
extern void free_table(int **table);
extern int get_click_rect(SDL_Point *point, SDL_Rect *tilemap, int *click_rect);
extern void update_screen
(
	SDL_Window *window,
	SDL_Renderer *renderer,
	int **table, 
	SDL_Rect (*tilemap)[N_COLUMNS],
	int game_state,
	SDL_Texture *textures[]
);
extern void shuffle(int *array, int n);
extern void place_bombs(int **minefield, int **bomb_coords);
extern void update_numbers(int **minefield, int **bomb_coords);
extern void load_textures(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *textures[]);
extern void cleanup_textures(SDL_Texture *textures[]);
extern void limit_fps(unsigned int limit);
extern void show_area(int **visible_board, int **game_board, int x, int y);

#endif