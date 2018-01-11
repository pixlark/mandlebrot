#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

typedef char bool;
#define false 0x00
#define true  0xFF

#define WIDTH  1920
#define HEIGHT 1080

/*
#define CENTER_X -0.1011
#define CENTER_Y  0.9563*/
#define CENTER_X -0.743643
#define CENTER_Y  0.131825

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

enum Mode {
	LIVE,
	RENDER,
};

int main(int argc, char ** argv)
{
	if (argc < 2) {
		printf("Please choose 'live' or 'render'\n");
		return 0;
	}
	
	enum Mode mode;
	
	if (strcmp(argv[1], "live") == 0) {
		mode = LIVE;
	} else if (strcmp(argv[1], "render") == 0) {
		mode = RENDER;
	} else {
		printf("Please choose 'live' or 'render'\n");
		return 0;
	}

	SDL_Window * window;
	SDL_Surface * wsurf;
	SDL_Surface * psurf = NULL;
	
	if (mode == LIVE) {
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow(
			"Visualizer",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
		wsurf = SDL_GetWindowSurface(window);
	}

	double zoom = 0.1;
	double zoom_max = 0.00000000298;
	double change = 0.95;

	int img_counter = 0;
	
	SDL_Event event;
	bool running = true;
	while (running) {
		while (mode == LIVE && SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}
		if (zoom <= zoom_max) running = false;

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
					color[0] = 256 - iter;
					color[1] = 256 - iter;
					color[2] = 256 - iter;
				}
				set_pixel(x, y, color);
			}
		}

		zoom *= change;

		if (mode == RENDER) {
			char name[64];
			sprintf(name, "img/m%04d.png", img_counter);
		
			stbi_write_png(name, WIDTH, HEIGHT, 4, pixels, 4*WIDTH);
			printf("%s rendered\n", name);

			img_counter++;
		} else if (mode == LIVE) {		
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
	}
	
	return 0;
}
