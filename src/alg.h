#ifndef MANDIBLE_ALG_H
#define MANDIBLE_ALG_H

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

#endif
