// Loosely based on https://medium.com/@fredhii/rising-the-ground-64957937513b

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"

#define N_COLS 6
#define N_ROWS 6
#define DEFNAME "file.dat"
#define DELIM " "
#define DIM_X(x,y) (inc * x) - (inc * y) + offx
#define DIM_Y(x,y,z) ((1 - inc) * x) + ((1 - inc) * y) - z + offy
#define A1 M_PI / 180
#define A2 -M_PI / 180
#define CX 640 / M_PI
#define CY 512 / M_PI

typedef struct {
	float x, y, z;
} col;

typedef struct {
	col cols[N_COLS];
} row;

typedef struct {
	row rows[N_ROWS];
} grid;

int main(int argc, char *argv[]) {

	int offx = 0;
	int offy = 0;
	float inc = 0.75;

	// Open file.
	char *fname = DEFNAME;
	if (argc > 1)
		fname = argv[1];
	FILE *fp = fopen(fname, "r");
	if (!fp) {
		perror("Error opening file");
		return 1;
	}

	// Parse coordinates out of file.
	size_t l_bufsiz = 256;
	char *line_buffer = malloc(l_bufsiz * sizeof(char));
	size_t num_chars;
	size_t row_idx = 0;
	size_t col_idx = 0;
	grid isog = {0};
	for (int i = 0; i < N_ROWS; i++) {
		row cur_row = {0};
		for (int j = 0; j < N_COLS; j++) {
			col cur_col = {0.0, 0.0, 0.0};
			cur_row.cols[j] = cur_col;
		}
		isog.rows[i] = cur_row;
	}
	while ((num_chars = getline(&line_buffer, &l_bufsiz, fp)) != -1) {
		if (row_idx == N_ROWS)
			break;
		if (num_chars < 2) {
			row_idx++;
			col_idx = 0;
			continue;
		}

		char tokbuf[l_bufsiz];
		strcpy(tokbuf, line_buffer);
		char *token = strtok(tokbuf, DELIM);
		for (int i = 0; i < 3; i++) {
			float f = atoi(token);
			if (!token)
				break;
			switch (i) {
			case 0:
				isog.rows[row_idx].cols[col_idx].x = f;
				break;
			case 1:
				isog.rows[row_idx].cols[col_idx].y = f;
				break;
			case 2:
				isog.rows[row_idx].cols[col_idx].z = f;
				break;
			}
			token = strtok(NULL, DELIM);
		}
		col_idx++;
	}
	fclose(fp);

	// Start SDL.
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("isog", 0, 0, 640, 512, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	const Uint8 *keyboard_state_array = SDL_GetKeyboardState(NULL);

	// Main loop.
	while (1) {
		// Get events.
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				break;
			if (e.type == SDL_KEYDOWN) {
				// Rotate left and right.
				if (keyboard_state_array[SDL_SCANCODE_LEFT]) {
					for (int i = 0; i < N_ROWS; i++) {
						for (int j = 0; j < N_COLS; j++) {
							float x = isog.rows[i].cols[j].x;
							float y = isog.rows[i].cols[j].y;
							isog.rows[i].cols[j].x = (x - CX) * cos(A2) - (y - CY) * sin(A2) + CX;
							isog.rows[i].cols[j].y = (x - CX) * sin(A2) + (y - CY) * cos(A2) + CY;
						}
					}
				}
				if (keyboard_state_array[SDL_SCANCODE_RIGHT]) {
					for (int i = 0; i < N_ROWS; i++) {
						for (int j = 0; j < N_COLS; j++) {
							float x = isog.rows[i].cols[j].x;
							float y = isog.rows[i].cols[j].y;
							isog.rows[i].cols[j].x = (x - CX) * cos(A1) - (y - CY) * sin(A1) + CX;
							isog.rows[i].cols[j].y = (x - CX) * sin(A1) + (y - CY) * cos(A1) + CY;
						}
					}
				}
				// Rotate up and down.
				if (keyboard_state_array[SDL_SCANCODE_UP])
					inc -= 0.01;
				if (keyboard_state_array[SDL_SCANCODE_DOWN])
					inc += 0.01;
				// Translate up and down.
				if (keyboard_state_array[SDL_SCANCODE_W])
					offy -= 8;
				if (keyboard_state_array[SDL_SCANCODE_S])
					offy += 8;
				// Translate left and right.
				if (keyboard_state_array[SDL_SCANCODE_A])
					offx -= 8;
				if (keyboard_state_array[SDL_SCANCODE_D])
					offx += 8;
			}
		}

		// Set colors and clear.
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	
		// Draw rows.
		for (int i = 0; i < N_ROWS; i++) {
			for (int j = 0; j < (N_COLS-1); j++) {
				col col1 = isog.rows[i].cols[j];
				col col2 = isog.rows[i].cols[j+1];
				SDL_RenderDrawLine(renderer, DIM_X(col1.x, col1.y), DIM_Y(col1.x, col1.y, col1.z),
					DIM_X(col2.x, col2.y), DIM_Y(col2.x, col2.y, col2.z));
			}
		}

		// Draw columns connecting rows.
		for (int i = 0; i < N_COLS; i++) {
			for (int j = 0; j < (N_ROWS-1); j++) {
				col col1 = isog.rows[j].cols[i];
				col col2 = isog.rows[j+1].cols[i];
				SDL_RenderDrawLine(renderer, DIM_X(col1.x, col1.y), DIM_Y(col1.x, col1.y, col1.z),
					DIM_X(col2.x, col2.y), DIM_Y(col2.x, col2.y, col2.z));
			}
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
