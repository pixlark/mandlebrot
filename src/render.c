#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

typedef char bool;
#define false 0x00
#define true  0xFF

#define BYTE_DEPTH 3

/*
 * Test coordinates
 *
 * -0.1011,0.9563
 *
 * -0.743643,0.131825
 *
 * 0.360240443437614,-0.641313061064803
 *
 * Not enough detail to do this one...?
 * -1.749998410993740,-0.000000000000001
 *
 * -0.701000092002025,0.351000000999792
*/

typedef struct {
	unsigned char * pixels;
	int width;
	int height;
} Pixels;

void set_pixel(Pixels pixels, int x, int y, unsigned char color[BYTE_DEPTH])
{
	if (x < 0 || x >= pixels.width || y < 0 || y >= pixels.height) return;
	for (int i = 0; i < BYTE_DEPTH; i++) {
		*(pixels.pixels + (x + y * pixels.width) * BYTE_DEPTH + i) = color[i];
	}
}

void clear_pixels(Pixels pixels)
{
	for (int i = 0; i < pixels.width * pixels.height * BYTE_DEPTH; i++) {
		pixels.pixels[i] = 0;
	}
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
int mandlebrot_diverges(double x, double y)
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

void print_usage()
{
	printf("Usage: mandible [options] file\n");
	printf("  %-20sSpecify coordinates to zoom in on. (Default: 0 + 0i)\n", "-p ypos,xpos");
	printf("  %-20sSpecify output resolution. (Default: 800x600)\n", "-r width,height");
	printf("  %-20sSpecify zoom multiplier for each frame. (Default: 2.0)\n", "-z mult");
	printf("  %-20sSpecify number of frames to render. (Default: indefinite)\n", "-f count");
}

int main(int argc, char ** argv)
{
	Pixels pixels;
	pixels.width = 800;
	pixels.height = 600;
	
	double pos_x = 0.0;
	double pos_y = 0.0;
	double change_rec = 2.0;
	int frame_max = -1;
	{
		// Parse command line args
		char opt;
		bool passed_pos = false;
		
		while ((opt = getopt(argc, argv, "p:r:z:f:h")) != -1) {
			switch (opt) {
			case 'p':
				if (sscanf(optarg, "%lf,%lf", &pos_x, &pos_y) != 2) {
					print_usage();
					return 1;
				}
				break;
			case 'r':
				if (sscanf(optarg, "%d,%d", &pixels.width, &pixels.height) != 2) {
					print_usage();
					return 1;
				}
				break;
			case 'z':
				if (sscanf(optarg, "%lf", &change_rec) != 1) {
					print_usage();
					return 1;
				}
				break;
			case 'f':
				if (sscanf(optarg, "%d", &frame_max) != 1) {
					print_usage();
					return 1;
				}
				break;
			case 'h':
				printf("The 'Mandible' Mandlebrot Renderer\n");
				printf("Written by pixlark\n");
				printf("https://pixlark.github.io/\n");
				print_usage();
				return 0;
			default:
				print_usage();
				return 1;
				break;
			}
		}
	}

	pixels.pixels = (unsigned char *)
		malloc(sizeof(unsigned char) * pixels.width * pixels.height * BYTE_DEPTH);
	
	double zoom     = 0.1;
	double change   = 1.0 / change_rec;

	int img_counter = 0;

	int num_cores = omp_get_num_procs();
	printf("%d available cores\n", num_cores);
	printf("Rendering zoom at pos %.3lf %.3lf with %.3lf magnification each frame\n",
		pos_x, pos_y, 1 / change);
	
	bool running = true;
	while (running) {

		clear_pixels(pixels);

		if (frame_max != -1 && img_counter >= frame_max) running = false;
		
		#pragma omp parallel for
		for (int y = -pixels.height/2; y < pixels.height/2; y++) {
			for (int x = -pixels.width/2; x < pixels.width/2; x++) {
				double dx = (double) (x * zoom) + pos_x;
				double dy = (double) (y * zoom) + pos_y;
				unsigned char color[BYTE_DEPTH];
				color[3] = 0xFF;
				int iter = mandlebrot_diverges(dx, dy);
				if (iter == -1) {
					color[0] = 0x00;
					color[1] = 0x00;
					color[2] = 0x00;
				} else {
					color[0] = iter;
					color[1] = iter;
					color[2] = iter;
				}
				set_pixel(pixels, x + (pixels.width  / 2), y + (pixels.height / 2), color);
			}
		}

		zoom *= change;
		
		char name[64];
		sprintf(name, "img/m%04d.bmp", img_counter);
		
		stbi_write_bmp(name, pixels.width, pixels.height, BYTE_DEPTH, pixels.pixels);
		printf("%s rendered\n", name);

		img_counter++;

	}
	
	return 0;
}
