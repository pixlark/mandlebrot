#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "alg.h"

typedef char bool;
#define false 0x00
#define true  0xFF

#define WIDTH  1920
#define HEIGHT 1080

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

int main(int argc, char ** argv)
{
	double zoom     = 0.1;
	double zoom_max = 0.000000000000001;
	double change   = 0.95;

	int img_counter = 0;

	int num_cores = omp_get_num_procs();
	printf("%d available cores\n", num_cores);

	//unsigned long int start_ticks = SDL_GetTicks(); // Find non-SDL alternative
	
	bool running = true;
	while (running) {
		//printf("%.15f\n", zoom);
		//if (zoom <= zoom_max) running = false;
		if (img_counter >= 350) running = false;

		#pragma omp parallel for
		for (int y = -HEIGHT/2; y < HEIGHT/2; y++) {
			for (int x = -WIDTH/2; x < WIDTH/2; x++) {
				double dx = (double) (x * zoom) + CENTER_X;
				double dy = (double) (y * zoom) + CENTER_Y;
				unsigned char color[4];
				color[3] = 0xFF;
				int iter = mandlebrot_diverges(dx, dy);
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
		
		char name[64];
		sprintf(name, "img/m%04d.png", img_counter);
		
		stbi_write_bmp(name, WIDTH, HEIGHT, 4, pixels);
		printf("%s rendered\n", name);

		img_counter++;

	}
	
	//printf("Took %lu ticks\n", SDL_GetTicks() - start_ticks);
	
	return 0;
}
