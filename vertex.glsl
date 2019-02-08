attribute vec3 coord3d;
attribute vec3 v_color;
uniform mat4 model_mat;
uniform mat4 proj_mat;
uniform mat4 view_mat;
varying vec3 f_color;

void main(void) {
	gl_Position = proj_mat * view_mat * model_mat * vec4(coord3d, 1.0);
	f_color = v_color;
}
