#include <iostream>
#include "SDL.h"

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 640;

const int CELL_ROW = 200, CELL_COL = 160;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int cell_arr[CELL_COL][CELL_ROW] = { 0 };
int state_arr[CELL_COL][CELL_ROW] = { 0 };

bool appInit() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "SDL init failed!\n";
		return false;
	}
	window = SDL_CreateWindow("game of life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (window == NULL) {
		std::cout << "window creation failed!\n"; 
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (window == NULL) {
		std::cout << "renderer creation failed!\n";
		return false;
	}
	return true;
}

int countNeighbor(int cell[CELL_COL][CELL_ROW], int col, int row)
{
	int sum = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (!(i == 0 && j == 0)) {
				int m_cols = (col + i + CELL_COL) % CELL_COL;
				int m_rows = (row + j + CELL_ROW) % CELL_ROW;

				sum += cell[m_cols][m_rows];
			}
		}
	}
	return sum;
}

void generateCells(int cell[CELL_COL][CELL_ROW], int state[CELL_COL][CELL_ROW]) {
	srand(time(NULL));
	for (int i = 0; i < CELL_COL; i++) {
		std::fill_n(state[i], CELL_ROW, 0);
		for (int j = 0; j < CELL_ROW; j++) {
			cell_arr[i][j] = rand() % 2;
		}
	}
}

int main(int argc, char *argv[])
{

	if (!appInit()) {
		std::cout << "SDL ERROR, " << SDL_GetError() << '\n';
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	bool isRunning = true;

	SDL_Event evnt;

	//cell_arr[0][1] = cell_arr[1][2] = cell_arr[2][0] = cell_arr[2][1] = cell_arr[2][2] = 1;
	
	generateCells(cell_arr, state_arr);

	SDL_Rect grid_destRect;
	grid_destRect.w = grid_destRect.h = 4;

	int counter = 0;
	uint32_t update_speed_ms = 500;
	uint32_t update_timer_last = SDL_GetTicks();
	uint32_t update_timer_now = SDL_GetTicks();

	uint32_t render_speed_ms = 1000 / 60;
	uint32_t render_timer_last = SDL_GetTicks();
	uint32_t render_timer_now = SDL_GetTicks();


	printf("Controls\n");
	printf("[G]: Generate cells\n");
	printf("[T]: Change the miliseconds per update\n");
	printf("[F]: Change the render FPS (Default: 60)\n");

	//Game loop start
	while (isRunning) {
		if (SDL_PollEvent(&evnt)) {
			switch (evnt.type) {
			case(SDL_QUIT): 
				isRunning = false;
				break;
			case(SDL_KEYDOWN):
				switch (evnt.key.keysym.sym) {
				case SDLK_g: {
					generateCells(cell_arr, state_arr);
					break;
				}
				case SDLK_t: {
					uint32_t new_speed_ms = 0;
					while (std::cout << "Enter new milliseconds per update: " && !(std::cin >> new_speed_ms)) {
						std::cin.clear();
						std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						std::cout << "Invalid input. please re-enter.\n";
					}
					update_speed_ms = new_speed_ms;
					break;
				}
				case SDLK_f: {
					uint32_t new_fps = 0;
					while (true) {
						while (std::cout << "Enter new render FPS: " && !(std::cin >> new_fps)) {
							std::cin.clear();
							std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
							std::cout << "Invalid input. please re-enter.\n";
						}
						if (new_fps > 0) {
							render_speed_ms = 1000 / new_fps;
							break;
						}
						else {
							printf("FPS must be over 0. please re-enter.\n");
						}
					}
					break;
				}
				default:
					break;
				}
			}
			if (SDL_QUIT == evnt.type) {
				isRunning = false;
			}
		}

		update_timer_now = SDL_GetTicks();

		if (update_timer_now - update_timer_last >= update_speed_ms) {
			for (int i = 0; i < CELL_COL; i++) {
				for (int j = 0; j < CELL_ROW; j++) {
					int neighbors = countNeighbor(cell_arr, i, j);

					if (cell_arr[i][j] == 1)
						state_arr[i][j] = neighbors == 2 || neighbors == 3;
					else
						state_arr[i][j] = neighbors == 3;

				}
			}

			for (int i = 0; i < CELL_COL; i++) {
				for (int j = 0; j < CELL_ROW; j++) {
					cell_arr[i][j] = state_arr[i][j];
				}
			}
			update_timer_last = update_timer_now;
		}
		
		
		//----------------------------------------\\
		//Rendering
		render_timer_now = SDL_GetTicks();

		if (render_timer_now - render_timer_last >= render_speed_ms) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			for (int i = 0; i < CELL_COL; i++) {
				for (int j = 0; j < CELL_ROW; j++) {
					if (cell_arr[i][j] == 1)
					{
						grid_destRect.x = 4 * j;
						grid_destRect.y = 4 * i;
						SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
						SDL_RenderFillRect(renderer, &grid_destRect);
					}
				}
			}
			SDL_RenderPresent(renderer);
			render_timer_last = render_timer_now;
		}
		//-----------------------------------------//
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}