#include <math.h>
#include <string.h>
#include "matrix5.h"

#ifndef M_PI
#define M_PI 3.14
#endif

#define M(m, r, c) (m)[((c) * 5) + (r)]


float Identity5[25] = IDENTITY5_MATRIX;

void matmul5(float *p, const float *a, const float *b)
{
#define P(r, c) M(p,r,c)
#define B(r, c) M(b,r,c)
	int i;

	for (i = 0; i < 5; i++) {
		const float ai0 = M(a,i,0), ai1 = M(a,i,1),
		      ai2 = M(a,i,2), ai3 = M(a,i,3), ai4 = M(a,i,4);

		P(i,0) = ai0*B(0,0) + ai1*B(1,0) + ai2*B(2,0) + ai3*B(3,0) +
			ai4*B(4,0);
		P(i,1) = ai0*B(0,1) + ai1*B(1,1) + ai2*B(2,1) + ai3*B(3,1) +
			ai4*B(4,1);
		P(i,2) = ai0*B(0,2) + ai1*B(1,2) + ai2*B(2,2) + ai3*B(3,2) +
			ai4*B(4,2);
		P(i,3) = ai0*B(0,3) + ai1*B(1,3) + ai2*B(2,3) + ai3*B(3,3) +
			ai4*B(4,3);
		P(i,4) = ai0*B(0,4) + ai1*B(1,4) + ai2*B(2,4) + ai3*B(3,4) +
			ai4*B(4,4);
	}
#undef P
#undef B
}

void matrix5_translate(float *m, float x, float y, float z, float w)
{
	m[20] = m[0]*x + m[5]*y + m[10]*z + m[15]*w + m[20];
	m[21] = m[1]*x + m[6]*y + m[11]*z + m[16]*w + m[21];
	m[22] = m[2]*x + m[7]*y + m[12]*z + m[17]*w + m[22];
	m[23] = m[3]*x + m[8]*y + m[13]*z + m[18]*w + m[23];
	m[24] = m[4]*x + m[9]*y + m[14]*z + m[19]*w + m[24];
}

void matrix5_scale(float *m, float x, float y, float z, float w)
{
	m[0] *= x; m[5] *= y; m[10] *= z; m[15] *= w;
	m[1] *= x; m[6] *= y; m[11] *= z; m[16] *= w;
	m[2] *= x; m[7] *= y; m[12] *= z; m[17] *= w;
	m[3] *= x; m[8] *= y; m[13] *= z; m[18] *= w;
	m[4] *= x; m[9] *= y; m[14] *= z; m[19] *= w;
}

void matrix5_rotate(float *m, float angle0, float angle1, float x, 
		float y, float z, float w)
{
	float s0, c0, s1, c1;
	float m2[25];

	s0 = sinf(angle0*M_PI/180.f);
	c0 = cosf(angle0*M_PI/180.f);
	s1 = sinf(angle1*M_PI/180.f);
	c1 = cosf(angle1*M_PI/180.f);


	memcpy(m2, Identity5, sizeof(float)*25);

#define rotate_2d(i, j, sign, k)	\
	do {					\
		M(m2,i,i) = c ## k;		\
		M(m2,j,j) = c ## k;		\
		if (sign > 0.f) {		\
			M(m2,i,j) = -s ## k;	\
			M(m2,j,i) = s ## k;	\
		} else {			\
			M(m2,i,j) = s ## k ;	\
			M(m2,j,i) = -s ## k;	\
		}				\
	} while (0)


	if (x == 0.f) {
		if (y == 0.f) {
			if (z == 0.f) {
				if (w != 0.f) {
					rotate_2d(2, 0, w, 0);
					rotate_2d(1, 2, w, 1);
					goto out;
				}
			} else if (w == 0.f) {
				rotate_2d(1, 3, z, 0);
				rotate_2d(3, 0, z, 1);
				goto out;
			}
		} else if (z == 0.f && w == 0.f) {
			rotate_2d(2, 3, y, 0);
			rotate_2d(0, 2, y, 1);
			goto out;
		}
	} else if (y == 0.f && z == 0.f && w == 0.f) {
		rotate_2d(1, 2, x, 0);
		rotate_2d(2, 3, x, 1);
		goto out;
	}
#undef rotate_2d

out:
	matmul5(m, m, m2);
}


/*
 * the projection matrix functions load the corresponding projection matrix, no
 * multiplication involved, no error checking
 * the camera is at the origin looking down the -w axis
 */
void matrix5_ortho(float *m)
{
	const float x = 2.f/(X_MAX - X_MIN),
	      y = 2.f/(Y_MAX - Y_MIN),
	      z = 2.f/(Z_MAX - Z_MIN),
	      w = -2.f/(W_MAX - W_MIN),
	      a = -(X_MAX + X_MIN)/(X_MAX - X_MIN),
	      b = -(Y_MAX + Y_MIN)/(Y_MAX - Y_MIN),
	      c = -(Z_MAX + Z_MIN)/(Z_MAX - Z_MIN),
	      d = -(W_MAX + W_MIN)/(W_MAX - W_MIN);

	m[0] = x;    m[5] = 0.f;  m[10] = 0.f;  m[15] = 0.f; m[20] = a;
	m[1] = 0.f;  m[6] = y;    m[11] = 0.f;  m[16] = 0.f; m[21] = b;
	m[2] = 0.f;  m[7] = 0.f;  m[12] = z;    m[17] = 0.f; m[22] = c;
	m[3] = 0.f;  m[8] = 0.f;  m[13] = 0.f;  m[18] = w;   m[23] = d;
	m[4] = 0.f;  m[9] = 0.f;  m[14] = 0.f;  m[19] = 0.f; m[24] = 1.f;
}

void matrix5_projection(float *m)
{
	memset(m, 0, sizeof(Identity5));

	M(m,0,0) = M(m,1,1) = M(m,2,2) = W_MIN;
	M(m,3,3) = W_MIN + W_MAX;
	M(m,3,4) = -W_MIN * W_MAX;
	M(m,4,3) = 1.f;
	M(m,4,4) = 0.f;
}

void matrix5_frustum(float *m)
{
	float m1[25], m2[25];

	matrix5_ortho(m1);
	matrix5_projection(m2);
	matmul5(m, m1, m2);
}

#undef M
