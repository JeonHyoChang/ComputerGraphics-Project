uniform mat4 u_pvm_matrix;

uniform mat4 u_vm_matrix;
uniform mat3 u_normal_matrix;

attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
varying vec3 v_vertex;
varying vec3 v_normal;

void main() {
  	v_texcoord = a_texcoord; 
	v_normal = normalize(u_normal_matrix*a_normal);
	v_vertex=(u_vm_matrix*a_vertex).xyz;
	gl_Position = u_pvm_matrix * a_vertex;
}