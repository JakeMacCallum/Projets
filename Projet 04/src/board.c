void update_screen
(
	SDL_Window *window,
	SDL_Renderer *renderer,
	int **table, 
	SDL_Rect (*tilemap)[N_COLUMNS],
	int game_state,
	SDL_Texture *textures[]
)
{
    switch (game_state)
	{
		case PLAYING:
		{
			for(int i = 0 ; i < N_ROWS ; i++)
				for(int j = 0 ; j < N_COLUMNS ; j++)
				{
					switch (table[i][j])
					{
					case HIDDEN:
						printf("first");
						SDL_Show(window, renderer, textures[HIDDEN], &tilemap[i][j]);
						break;
					case NOTHING:
						printf("second");
						SDL_Show(window, renderer, textures[NOTHING], DEBUGGING);
						break;
					case ONE:
						SDL_Show(window, renderer, textures[ONE], DEBUGGING);
						break;
					case TWO:
						SDL_Show(window, renderer, textures[TWO], DEBUGGING);
						break;
					case THREE:
						SDL_Show(window, renderer, textures[THREE], DEBUGGING);
						break;
					case FOUR:
						SDL_Show(window, renderer, textures[FOUR], DEBUGGING);
						break;
					case FIVE:
						SDL_Show(window, renderer, textures[FIVE], DEBUGGING);
						break;
					case SIX:
						SDL_Show(window, renderer, textures[SIX], DEBUGGING);
						break;
					case SEVEN:
						SDL_Show(window, renderer, textures[SEVEN], DEBUGGING);
						break;
					case EIGHT:
						SDL_Show(window, renderer, textures[EIGHT], DEBUGGING);
						break;
					case FLAG:
						SDL_Show(window, renderer, textures[FLAG], DEBUGGING);
						break;
					case QUESTION:
						SDL_Show(window, renderer, textures[QUESTION], DEBUGGING);
						break;
					default:
						break;
					}
				}
				break;
		}
		case LOSE:
		{
			break;
		}
		case WIN:
		{
			break;
		}
	}
}