/*
 * 4x4 matrix operations from mesa
 * /math/m_matrix.c
 * stored in memory as column major
 */
#include <math.h>
#include <string.h>
#include "matrix.h"

#ifndef M_PI
#define M_PI 3.14
#endif

#define M(m, r, c) (m)[((c) << 2) + (r)]

float Identity[16] = IDENTITY_MATRIX;

void matmul4(float *p, const float *a, const float *b)
{
#define P(r, c) M(p,r,c)
#define B(r, c) M(b,r,c)
	int i;

	for (i = 0; i < 4; i++) {
		const float ai0 = M(a,i,0), ai1 = M(a,i,1),
		      ai2 = M(a,i,2), ai3 = M(a,i,3);

		P(i,0) = ai0*B(0,0) + ai1*B(1,0) + ai2*B(2,0) + ai3*B(3,0);
		P(i,1) = ai0*B(0,1) + ai1*B(1,1) + ai2*B(2,1) + ai3*B(3,1);
		P(i,2) = ai0*B(0,2) + ai1*B(1,2) + ai2*B(2,2) + ai3*B(3,2);
		P(i,3) = ai0*B(0,3) + ai1*B(1,3) + ai2*B(2,3) + ai3*B(3,3);
	}
#undef P
#undef B
}


void matmul34(float *p, const float *a, const float *b)
{
	int i;
	for (i = 0; i < 3; i++) {
		const float ai0 = M(a,i,0), ai1 = M(a,i,1),
		      ai2 = M(a,i,2), ai3 = M(a,i,3);

		M(p,i,0) = ai0*M(b,0,0) + ai1*M(b,1,0) + ai2*M(b,2,0);
		M(p,i,1) = ai0*M(b,0,1) + ai1*M(b,1,1) + ai2*M(b,2,1);
		M(p,i,2) = ai0*M(b,0,2) + ai1*M(b,1,2) + ai2*M(b,2,2);
		M(p,i,3) = ai0*M(b,0,3) + ai1*M(b,1,3) + ai2*M(b,2,3) + ai3;
	}
	M(p,3,0) = 0; M(p,3,1) = 0; M(p,3,2) = 0; M(p,3,3) = 1;
}

/*
 * M = MT
 * where T = {
 * {1,0,0,x},
 * {0,1,0,y},
 * {0,0,1,z},
 * {0,0,0,1}
 * }
 */
void matrix_translate(float *m, float x, float y, float z)
{
	m[12] = m[0]*x + m[4]*y + m[8]*z + m[12];
	m[13] = m[1]*x + m[5]*y + m[9]*z + m[13];
	m[14] = m[2]*x + m[6]*y + m[10]*z + m[14];
	m[15] = m[3]*x + m[7]*y + m[11]*z + m[15];
}

/*
 * M = MS
 * where S = {
 * {x,0,0,0},
 * {0,y,0,0},
 * {0,0,z,0},
 * {0,0,0,1}
 * }
 */
void matrix_scale(float *m, float x, float y, float z)
{
	m[0] *= x; m[4] *= y; m[8] *= z;
	m[1] *= x; m[5] *= y; m[9] *= z;
	m[2] *= x; m[6] *= y; m[10] *= z;
	m[3] *= x; m[7] *= y; m[11] *= z;
}

void matrix_rotate(float *m, float angle, float x, float y, float z)
{
	float s, c;
	float m2[16];

	s = sinf(angle*M_PI/180.f);
	c = cosf(angle*M_PI/180.f);

	memcpy(m2, Identity, sizeof(float)*16);

#define rotate_2d(i, j, sign)	\
do {					\
	M(m2,i,i) = c;			\
	M(m2,j,j) = c;			\
	if (sign > 0.f) {		\
		M(m2,i,j) = -s;		\
		M(m2,j,i) = s;		\
	} else {			\
		M(m2,i,j) = s;		\
		M(m2,j,i) = -s;		\
	}				\
} while (0)


	if (x == 0.f) {
		if (y == 0.f) {
			if (z != 0.f) {
				rotate_2d(0, 1, z);
				goto out;
			}
		} else if (z == 0.f) {
			rotate_2d(2, 0, y);
			goto out;
		}
	} else if (y == 0.f && z == 0.f) {
		rotate_2d(1, 2, x);
		goto out;
	}
#undef rotate_2d
	
out:
	matmul4(m, m, m2);
}

/*
 * the projection matrix functions load the corresponding projection matrix, no
 * multiplication involved, no error checking
 * near, far > 0
 * but represent -ve
 *
 */
void matrix_frustum(float *m, float left, float right,
		float bottom, float top,
		float near, float far)
{
	const float x = 2.f*near/(right - left),
	      y = 2.f*near/(top - bottom),
	      a = (right + left)/(right - left),
	      b = (top + bottom)/(top - bottom),
	      c = -(far + near)/(far - near),
	      d = -2.f*far*near/(far - near);

	m[0] = x;    m[4] = 0.f;  m[8] = a;      m[12] = 0.f;
	m[1] = 0.f;  m[5] = y;    m[9] = b;      m[13] = 0.f;
	m[2] = 0.f;  m[6] = 0.f;  m[10] = c;     m[14] = d;
	m[3] = 0.f;  m[7] = 0.f;  m[11] = -1.f;  m[15] = 0.f;
}

void matrix_ortho(float *m, float left, float right, float bottom, float top,
		float near, float far)
{
	const float x = 2.f/(right - left),
	      y = 2.f/(top - bottom),
	      z = -2.f/(far - near),
	      a = -(right + left)/(right - left),
	      b = -(top + bottom)/(top - bottom),
	      c = -(far + near)/(far - near);

	m[0] = x;    m[4] = 0.f;  m[8] = 0.f;   m[12] = a;
	m[1] = 0.f;  m[5] = y;    m[9] = 0.f;   m[13] = b;
	m[2] = 0.f;  m[6] = 0.f;  m[10] = z;    m[14] = c;
	m[3] = 0.f;  m[7] = 0.f;  m[11] = 0.f;  m[15] = 1.f;
}

void matrix_perspective(float *m, float fovy, float aspect,
		float near, float far)
{
	const float top = near * tan(M_PI/180 * fovy/2),
	      bottom = -top,
	      right = aspect*top,
	      left = -right;
	matrix_frustum(m, left, right, bottom, top, near, far);
}

#undef M
