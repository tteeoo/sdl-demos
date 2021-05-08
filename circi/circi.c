#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "SDL.h"

// Radius of the circle.
#define R 2
#define R2 (R * R)
#define D (R * 2)
// Conversion rate between units/pixels.
#define PR 60

typedef struct point point;
struct point {
	point *next;
	double x;
	double y;
};

// Return the inverted point for a given point.
point *circi(point *p) {
	point *ci = (point*)malloc(sizeof(point));
	ci->next = NULL;
	ci->x = (p->x*R2) / ((p->x*p->x) + (p->y*p->y));
	ci->y = (p->y*R2) / ((p->x*p->x) + (p->y*p->y));
	return ci;
}

// Frees points.
void freepoints(point *head) {
	point *tmp;
	while (head != NULL) {
		tmp = head;
		head = head->next;
		free(tmp);
	}
}

// Draws a circle.
// Based on https://stackoverflow.com/questions/38334081/howto-draw-circles-arcs-and-vector-graphics-in-sdl#answer-48291620
void drawcircle(SDL_Renderer *renderer, int cx, int cy) {

	int x = ((R*PR) - 1);
	int y = 0;
	int tx = 1;
	int ty = 1;
	int error = (tx - (D*PR));

	while (x >= y) {
		SDL_RenderDrawPoint(renderer, cx + x, cy - y);
		SDL_RenderDrawPoint(renderer, cx + x, cy + y);
		SDL_RenderDrawPoint(renderer, cx - x, cy - y);
		SDL_RenderDrawPoint(renderer, cx - x, cy + y);
		SDL_RenderDrawPoint(renderer, cx + y, cy - x);
		SDL_RenderDrawPoint(renderer, cx + y, cy + x);
		SDL_RenderDrawPoint(renderer, cx - y, cy - x);
		SDL_RenderDrawPoint(renderer, cx - y, cy + x);

		if (error <= 0) {
			y++;
			error += ty;
			ty += 2;
		} 
		if (error > 0) {
			x--;
			tx += 2;
			error += (tx - (D*PR));
		}
	}
}

// Draws a point (creating a 3x3 square).
void drawpoint(SDL_Renderer *renderer, point *p)  {
	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10)-1, (-p->y * PR) + (PR*10)-1);
	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10), (-p->y * PR) + (PR*10)-1);
	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10)+1, (-p->y * PR) + (PR*10)-1);

	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10)-1, (-p->y * PR) + (PR*10));
	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10), (-p->y * PR) + (PR*10)); // Center
	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10)+1, (-p->y * PR) + (PR*10));

	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10)-1, (-p->y * PR) + (PR*10)+1);
	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10), (-p->y * PR) + (PR*10)+1);
	SDL_RenderDrawPoint(renderer, (p->x * PR) + (PR*10)+1, (-p->y * PR) + (PR*10)+1);
}

int main() {

	// Start SDL (create window and renderer).
	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("circi", 500, 500, PR*20, PR*20, 0);
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	// Head of linked list of points.
	point *head = NULL;

	int mx, my;
	int down = 0;

	// Main loop
	while (1) {

		// Detect events
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				break;
			// Clear the points if a key is pressed
			if (e.type == SDL_KEYDOWN) {
				freepoints(head);
				head = NULL;
			}
			// Set down to 1 while the mouse is pressed.
			if (e.type == SDL_MOUSEBUTTONDOWN)
				down = 1;
			if (e.type == SDL_MOUSEBUTTONUP)
				down = 0;
		}
		// Create a new point at the mouse location and its inverse if down.
		if (down) {
			SDL_GetMouseState(&mx, &my);
			point *new = (point*)malloc(sizeof(point));
			point *newi;
			new->x = ((double)mx - 500) / 50;
			new->y = ((double)my - 500) / -50;
			newi = circi(new);
			new->next = newi;

			if (head != NULL) {
				point *current = head;
				while (current->next != NULL) {
					current = current->next;
				}
				current->next = new;
			} else
				head = new;
		}

		// Clear and set colors.
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	
		// Draw unit circle and lines.
		drawcircle(renderer, PR*10, PR*10);
		SDL_RenderDrawLine(renderer, PR*10, 0, PR*10, PR*20);
		SDL_RenderDrawLine(renderer, 0, PR*10, PR*20, PR*10);

		// Draw points.
		if (head != NULL) {
			point *current = head;
			while (current != NULL) {
				// Do not draw points that are off the screen.
				if (current->x < 10 && current->y < 10)
					drawpoint(renderer, current);
				current = current->next;
			}
		}

		SDL_RenderPresent(renderer);
	}

	// Clean up
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	freepoints(head);

	return 0;
}
