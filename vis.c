#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

typedef char bool;
#define false 0x00
#define true  0xFF

#define WIDTH  1920
#define HEIGHT 1080

#define THREADED 1

#if 0
#define CENTER_X -0.1011
#define CENTER_Y  0.9563

#define CENTER_X -0.743643
#define CENTER_Y  0.131825

#define CENTER_X  0.360240443437614 // 15 digits of precision
#define CENTER_Y -0.641313061064803

#define CENTER_X -1.749998410993740 // Not enough detail to do this one...?
#define CENTER_Y -0.000000000000001
#endif

#define CENTER_X -0.701000092002025
#define CENTER_Y  0.351000000999792

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
#define ITER_MULT 1 // This doesn't seem to work...?
int diverges(double x, double y)
{
	double z[2] = {0, 0};
	for (int i = 0; i < MAX_ITERATIONS * ITER_MULT; i++) {
		square_complex(z);
		z[0] += x;
		z[1] += y;
		if (z[0] * z[0] + z[1] * z[1] > 4) {
			return (i / ITER_MULT);
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

	SDL_Init(0);
	SDL_Window  * window;
	SDL_Surface * wsurf;
	SDL_Surface * psurf = NULL;
	
	if (mode == LIVE) {
		SDL_Quit();
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow(
			"Visualizer",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
		wsurf = SDL_GetWindowSurface(window);
	}

	double zoom     = 0.1;
	double zoom_max = 0.000000000000001;
	double change   = 0.95;

	int img_counter = 0;

	int num_cores = omp_get_num_procs();

	unsigned long int start_ticks = SDL_GetTicks();
	
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
		//printf("%.15f\n", zoom);
		//if (zoom <= zoom_max) running = false;
		if (img_counter >= 300) running = false;
		
		/*
		int ymin = 0;
		int ymax = (i + 1) * (HEIGHT / num_cores);
		ymin -= HEIGHT / 2;
		ymax -= HEIGHT / 2;
		
		int xmin = -WIDTH  / 2;
		int xmax =  WIDTH  / 2;*/

		#if THREADED
		#pragma omp parallel for
		#endif
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
		
			//stbi_write_png(name, WIDTH, HEIGHT, 4, pixels, 4*WIDTH);
			stbi_write_bmp(name, WIDTH, HEIGHT, 4, pixels);
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

	if (mode == RENDER) {
		printf("Took %lu ticks while %s\n",
			SDL_GetTicks() - start_ticks,
			THREADED ? "threaded" : "unthreaded");
	}
	
	return 0;
}
