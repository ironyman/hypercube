#ifndef _MATRIX5_H
#define _MATRIX5_H

#define transform_point5(Q, M, P)					     \
do {									     \
	Q[0] = M[0]*P[0] + M[5]*P[1] + M[10]*P[2] + M[15]*P[3] + M[20]*P[4]; \
	Q[1] = M[1]*P[0] + M[6]*P[1] + M[11]*P[2] + M[16]*P[3] + M[21]*P[4]; \
	Q[2] = M[2]*P[0] + M[7]*P[1] + M[12]*P[2] + M[17]*P[3] + M[22]*P[4]; \
	Q[3] = M[3]*P[0] + M[8]*P[1] + M[13]*P[2] + M[18]*P[3] + M[23]*P[4]; \
	Q[4] = M[4]*P[0] + M[9]*P[1] + M[14]*P[2] + M[19]*P[3] + M[24]*P[4]; \
} while (0) 


#define IDENTITY5_MATRIX  {		\
	1.0, 0.0, 0.0, 0.0, 0.0,	\
	0.0, 1.0, 0.0, 0.0, 0.0,	\
	0.0, 0.0, 1.0, 0.0, 0.0,	\
	0.0, 0.0, 0.0, 1.0, 0.0,	\
	0.0, 0.0, 0.0, 0.0, 1.0		\
}

#define X_MIN -1.f
#define X_MAX 1.f
#define Y_MIN -1.f
#define Y_MAX 1.f
#define Z_MIN -1.f
#define Z_MAX 1.f
#define W_MIN 1.f
#define W_MAX 10.f

extern float Identity5[25];

void matrix5_frustum(float *m);
void matrix5_rotate(float *m, float angle0, float angle1, float x, 
		float y, float z, float w);
void matrix5_translate(float *m, float x, float y, float z, float w);

#endif
