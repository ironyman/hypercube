#ifndef _MATRIX_H
#define _MATRIX_H

#define transform_point(Q, M, P)				\
do {								\
	Q[0] = M[0]*P[0] + M[4]*P[1] + M[8]*P[2] +  M[12]*P[3];	\
	Q[1] = M[1]*P[0] + M[5]*P[1] + M[9]*P[2] +  M[13]*P[3];	\
	Q[2] = M[2]*P[0] + M[6]*P[1] + M[10]*P[2] + M[14]*P[3];	\
	Q[3] = M[3]*P[0] + M[7]*P[1] + M[11]*P[2] + M[15]*P[3]; \
} while (0) 

#define transform_point3(Q, M, P)				\
do {								\
	Q[0] = M[0]*P[0] + M[4]*P[1] + M[8]*P[2] +  M[12];	\
	Q[1] = M[1]*P[0] + M[5]*P[1] + M[9]*P[2] +  M[13];	\
	Q[2] = M[2]*P[0] + M[6]*P[1] + M[10]*P[2] + M[14];	\
	Q[3] = M[3]*P[0] + M[7]*P[1] + M[11]*P[2] + M[15];	\
} while (0)

#define TRANSFORM_NORMAL(TO, N, MAT)				\
do {								\
	TO[0] = N[0]*MAT[0] + N[1]*MAT[1] + N[2]*MAT[2];	\
	TO[1] = N[0]*MAT[4] + N[1]*MAT[5] + N[2]*MAT[6];	\
	TO[2] = N[0]*MAT[8] + N[1]*MAT[9] + N[2]*MAT[10];	\
} while (0)


#define IDENTITY_MATRIX  {		\
	1.0, 0.0, 0.0, 0.0,		\
	0.0, 1.0, 0.0, 0.0,		\
	0.0, 0.0, 1.0, 0.0,		\
	0.0, 0.0, 0.0, 1.0		\
}


extern float Identity[16];

void matmul4(float *p, const float *a, const float *b);
void matmul34(float *p, const float *a, const float *b);

void matrix_translate(float *m, float x, float y, float z);
void matrix_scale(float *m, float x, float y, float z);
void matrix_rotate(float *m, float angle, float x, float y, float z);

void matrix_frustum(float *m, float left, float right,
		float bottom, float top,
		float near, float far);

void matrix_ortho(float *m, float left, float right, float bottom, float top,
		float near, float far);

void matrix_perspective(float *m, float fovy, float aspect,
		float near, float far);

#endif
