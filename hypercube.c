#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/freeglut.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "matrix.h"
#include "matrix5.h"

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

#define TIMER_HZ 30

float model_mat[16] = IDENTITY_MATRIX;
float view_mat[16] = IDENTITY_MATRIX;
float proj_mat[16] = IDENTITY_MATRIX;
GLint model_uniform, view_uniform, proj_uniform;
float deg = 48;
float deg4 = 0;

GLuint program;
GLuint vertex_shader, fragment_shader;
GLint attrib_coord3d, attrib_color;

float *hypercube_buf;
int hypercube_buf_size = 3*16*sizeof(float);
GLuint hypercube_ibo, hypercube_vbo, hypercube_color_vbo;

float proj5[25] = IDENTITY5_MATRIX;
float modelview5[25] = IDENTITY5_MATRIX;

float hypercube5[] = { 
	/* front cube */
	/* front face 0*/
	1.f,   1.f,   1.f,   1.f,   1.f,
	-1.f,  1.f,   1.f,   1.f,   1.f,
	-1.f,  -1.f,  1.f,   1.f,   1.f,
	1.f,   -1.f,  1.f,   1.f,   1.f,

	/* back face 4 */
	1.f,   1.f,   -1.f,  1.f,   1.f,
	-1.f,  1.f,   -1.f,  1.f,   1.f,
	-1.f,  -1.f,  -1.f,  1.f,   1.f,
	1.f,   -1.f,  -1.f,  1.f,   1.f,

	/* back cube 8 */
	1.f,   1.f,   1.f,   -1.f,  1.f,
	-1.f,  1.f,   1.f,   -1.f,  1.f,
	-1.f,  -1.f,  1.f,   -1.f,  1.f,
	1.f,   -1.f,  1.f,   -1.f,  1.f,

	/* 12 */
	1.f,   1.f,   -1.f,  -1.f,  1.f,
	-1.f,  1.f,   -1.f,  -1.f,  1.f,
	-1.f,  -1.f,  -1.f,  -1.f,  1.f,
	1.f,   -1.f,  -1.f,  -1.f,  1.f,
};

float hypercube_colors[] = {
	1, 1, 1,
	1, 1, 0,
	1, 0, 1,
	1, 0, 0,
	0, 1, 1,
	0, 1, 0,
	0, 0, 1,
	0, 0, 0,
	1, 1, 1,
	1, 1, 1,
	1, 1, 1,
	1, 1, 1,
	1, 1, 1,
	1, 1, 1,
	1, 1, 1,
	1, 1, 1,
};

/* line segments */
unsigned char hypercube_index[] = {
	/* front cube */

	/* front face */
	0, 1, 1, 2, 2, 3, 3, 0, 
	/* back face */
	4, 5, 5, 6, 6, 7, 7, 4,
	/* top face */
	0, 4, 4, 5, 5, 1, 1, 0,
	/* bottom face */
	2, 6, 6, 7, 7, 3, 3, 2,
	/* left face */
	1, 5, 5, 6, 6, 2, 2, 1,
	/* right face */
	0, 3, 3, 7, 7, 4, 4, 0,

	/* back cube */

	/* front face */
	8, 9, 9, 10, 10, 11, 11, 8, 
	/* back face */
	12, 13, 13, 14, 14, 15, 15, 12,
	/* top face */
	8, 12, 12, 13, 13, 9, 9, 8,
	/* bottom face */
	10, 14, 14, 15, 15, 11, 11, 10,
	/* left face */
	9, 13, 13, 14, 14, 10, 10, 9,
	/* right face */
	8, 11, 11, 15, 15, 12, 12, 8,

	/* connect the corners */
	0, 8, 1, 9, 2, 10, 3, 11, 4, 12,
	5, 13, 6, 14, 7, 15, 
};

void project_hypercube(float *buf3, float *hypercube5)
{
	float tmp[5], tmp2[5];
	float mat[25];
	int i;

	for (i = 0; i < 16; ++i) { 
		float *p = buf3 + 3*i;
		float *p0 = hypercube5 + 5*i;

		transform_point5(tmp2, modelview5, p0);
		transform_point5(tmp, proj5, tmp2);

		p[0] = tmp[0]/tmp[4]; p[1] = tmp[1]/tmp[4];
		p[2] = tmp[2]/tmp[4];
	}
}

int init_hypercube()
{
	hypercube_buf = malloc(hypercube_buf_size);
	if (!hypercube_buf)
		return -1;

	matrix5_frustum(proj5);

	project_hypercube(hypercube_buf, hypercube5);

	glGenBuffers(1, &hypercube_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hypercube_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hypercube_index), 
			hypercube_index, GL_STATIC_DRAW);

	glGenBuffers(1, &hypercube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, hypercube_vbo);
	glBufferData(GL_ARRAY_BUFFER, hypercube_buf_size, hypercube_buf,
			GL_DYNAMIC_DRAW);

	glGenBuffers(1, &hypercube_color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, hypercube_color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(hypercube_colors),
			hypercube_colors, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return 0;
}

void destroy_hypercube()
{
	free(hypercube_buf);
	glDeleteBuffers(1, &hypercube_ibo);
	glDeleteBuffers(1, &hypercube_vbo);
	glDeleteBuffers(1, &hypercube_color_vbo);
}

void display_func()
{
	int size;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	glEnableVertexAttribArray(attrib_coord3d);
	glEnableVertexAttribArray(attrib_color);

	glBindBuffer(GL_ARRAY_BUFFER, hypercube_vbo);
	glVertexAttribPointer(attrib_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glBindBuffer(GL_ARRAY_BUFFER, hypercube_color_vbo);
	glVertexAttribPointer(attrib_color, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hypercube_ibo);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_LINES, size/sizeof(GLubyte), GL_UNSIGNED_BYTE, 
			0);

	glDisableVertexAttribArray(attrib_coord3d);
	glDisableVertexAttribArray(attrib_color);

	glUseProgram(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glutSwapBuffers();
}

void special_keybd_func(int key, int x, int y)
{
	(void)x, (void)y;

	switch (key) {
	case GLUT_KEY_LEFT:
		deg -= 3.f;
		printf("%f\r", deg);
		fflush(stdout);
		break;
	case GLUT_KEY_RIGHT:
		deg += 3.f;
		printf("%f\r", deg);
		fflush(stdout);
		break;
	}
}
void keybd_func(unsigned char key, int x, int y)
{
	(void)x, (void)y;

	switch (key) {
	case 'q':
	case 'Q':
		glutLeaveMainLoop();
		break;
	case ' ':
		deg4 += 5;
		printf("%f\r", deg4);
		fflush(stdout);
		break;
	}
}

void timer_func(int val)
{
	(void)val;
	static float counter;

	memcpy(model_mat, Identity, sizeof(Identity));

	matrix_rotate(model_mat, deg, 0, 1, 0);
	glUseProgram(program);
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, model_mat);
	glUseProgram(0);

	memcpy(modelview5, Identity5, sizeof(Identity5));
	matrix5_translate(modelview5, 0, 0, 0, 2);

	matrix5_rotate(modelview5, 0, deg4, 0, 0, 1, 0);

	matrix5_rotate(modelview5, 0, counter, 0, 0, 1, 0);
	counter += 1;
	
	project_hypercube(hypercube_buf, hypercube5);
	glBindBuffer(GL_ARRAY_BUFFER, hypercube_vbo);
	glBufferData(GL_ARRAY_BUFFER, hypercube_buf_size, hypercube_buf,
			GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glutPostRedisplay();
	glutTimerFunc(1.f/TIMER_HZ*1000, timer_func, 0);
}

int opengl_sucks()
{
	char *ver;
	int major, minor;

	ver = (char *)glGetString(GL_VERSION);
	if (sscanf(ver, "%d.%d", &major, &minor) != 2) 
		return -1;

	printf("OpenGL %d.%d\n", major, minor);
	if (major < 2 || (major == 2 && minor < 1)) {
		printf("need at least OpenGL 2.1\n");
		return -1;
	}

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		return -1;

	return 0;
}

char *slurp_file(char *name)
{
	char *buf;
	long size;
	FILE *fp;

	fp = fopen(name, "rb");
	if (!fp) 
		return NULL;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	buf = malloc((size + 1)*sizeof(char));
	buf[size] = 0;

	fseek(fp, 0, SEEK_SET);
	if (fread(buf, size, 1, fp) != 1) {
		free(buf);
		return NULL;
	}

	return buf;
}

void put_shader_error(GLuint shader)
{
	GLint length;
	char *log;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	log = malloc(length*sizeof(char));
	glGetShaderInfoLog(shader, length, &length, log);
	puts(log);
	free(log);
}

void put_program_error(GLuint program)
{
	int length;
	char *log;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
	log = malloc(length*sizeof(char));
	glGetProgramInfoLog(program, length, &length, log);
	puts(log);
	free(log);
}

int init_shaders()
{
	int error;
	char *buf;

	program = glCreateProgram();
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	buf = slurp_file("vertex.glsl");
	if (!buf)
		return -1;
	glShaderSource(vertex_shader, 1, (const char **)&buf, NULL);
	free(buf);

	glCompileShader(vertex_shader);

	buf = slurp_file("fragment.glsl");
	if (!buf) 
		return -1;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, (const char **)&buf, NULL);
	free(buf);

	glCompileShader(fragment_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &error);
	if (!error) {
		put_shader_error(vertex_shader);
		return -1;
	}
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &error);
	if (!error) {
		put_shader_error(fragment_shader);
		return -1;
	}

	glAttachShader(program, vertex_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &error);
	if (!error) {
		put_program_error(program);
		return -1;
	}

	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &error);
	if (!error) {
		put_program_error(program);
		return -1;
	}

	model_uniform = glGetUniformLocation(program, "model_mat");
	view_uniform = glGetUniformLocation(program, "view_mat");
	proj_uniform = glGetUniformLocation(program, "proj_mat");

	attrib_coord3d = glGetAttribLocation(program, "coord3d");
	attrib_color = glGetAttribLocation(program, "v_color");

	glUseProgram(program);
	glUniformMatrix4fv(proj_uniform, 1, GL_FALSE, proj_mat);
	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, view_mat);
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, model_mat);
	glUseProgram(0);

	return 0;
}

void destroy_shaders()
{
	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(program);
}

int main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(200, 100);

	glutInitContextVersion (2,1);
	glutInitContextProfile ( GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
			GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutCreateWindow("glut prototype");

	glutKeyboardFunc(keybd_func);
	glutSpecialFunc(special_keybd_func);
	glutDisplayFunc(display_func);
	glutTimerFunc(1.f/TIMER_HZ*1000, timer_func, 0);

	if (opengl_sucks() == -1)
		return -1;

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	matrix_perspective(proj_mat, 60.f, SCREEN_WIDTH/(float)SCREEN_HEIGHT,
			1.f, 100.f);
	matrix_translate(view_mat, 0.f, 0.f, -5.f);

	if (init_shaders() == -1)
		return -1;

	if (init_hypercube() == -1)
		return -1;


	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glutMainLoop();

	destroy_shaders();
	destroy_hypercube();

	return 0;
}
