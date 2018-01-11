#include <stdio.h>
#include <stdlib.h>

typedef char bool;
#define false 0x00
#define true  0xFF

#define WIDTH  800
#define HEIGHT 600

#define CENTER_X -0.1011
#define CENTER_Y  0.9563

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

static unsigned char pixels[WIDTH*HEIGHT][4];

void set_pixel(int x, int y, unsigned char color[4])
{
	x += (WIDTH  / 2) - CENTER_X;
	y += (HEIGHT / 2) - CENTER_Y;
	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
	for (int i = 0; i < 4; i++) {
		pixels[x + y*WIDTH][i] = color[i];
	}
}

// TODO(pixlark): Return copy, not reference
unsigned char * get_pixel(int x, int y)
{
	return pixels[x + y*WIDTH];
}

void square_complex(double z[2])
{
	double nz[2];
	nz[0] = z[0] * z[0] + -1 * z[1] * z[1];
	nz[1] = 2 * z[0] * z[1];
	z[0] = nz[0];
	z[1] = nz[1];
}

#define MAX_ITERATIONS 256
int diverges(double x, double y)
{
	double z[2] = {0, 0};
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		square_complex(z);
		z[0] += x;
		z[1] += y;
		if (z[0] * z[0] + z[1] * z[1] > 4) {
			return i;
		}
	}
	return -1;
}

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window * window = SDL_CreateWindow(
		"Visualizer",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Surface * wsurf = SDL_GetWindowSurface(window);

	SDL_Surface * psurf = NULL;

	double zoom = 0.1;
	double change = 0.75;
	
	SDL_Event event;
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}

		for (int y = -HEIGHT/2; y < HEIGHT/2; y++) {
			for (int x = -WIDTH/2; x < WIDTH/2; x++) {
				double dx = (double) (x * zoom) + CENTER_X;
				double dy = (double) (y * zoom) + CENTER_Y;
				unsigned char color[4];
				color[3] = 0xFF;
				int iter = diverges(dx, dy);
				if (iter == -1) {
					color[0] = 0x00;
					color[1] = 0x00;
					color[2] = 0x00;
				} else {
					color[0] = iter;
					color[1] = iter;
					color[2] = iter;
				}
				set_pixel(x, y, color);
			}
		}

		zoom *= change;
		
		SDL_FreeSurface(psurf);
		SDL_Surface * psurf = SDL_CreateRGBSurfaceFrom(
			pixels, WIDTH, HEIGHT, 32, 4*WIDTH,
			0x000000FF,
			0x0000FF00,
			0x00FF0000,
			0xFF000000);
		
		SDL_BlitSurface(psurf, NULL, wsurf, NULL);
		SDL_UpdateWindowSurface(window);
	}
	
	return 0;
}
