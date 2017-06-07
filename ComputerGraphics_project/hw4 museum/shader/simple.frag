//precision mediump float;


uniform vec3 light_position;
uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;
uniform sampler2D u_texid;

varying vec2 v_texcoord;
varying vec3 v_vertex;
varying vec3 v_normal;

vec4 directional_light() {
	vec4 color=vec4(0);
	vec3 normal=normalize(v_normal);
	vec3 light_vector=normalize(light_position);
	vec3 reflect_vector = reflect(-light_vector, normal);
	vec3 view_vector = normalize(-v_vertex);

	color += (light_ambient*material_ambient);

	float ndotl = max(0.0, dot(normal,light_vector));
	color+=(ndotl*light_diffuse*material_diffuse);

	float rdotv = max(0.0, dot(reflect_vector, view_vector));
	color += (pow(rdotv, material_shininess)*light_specular*material_specular);

	return color;
}


void main() {
 // gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
gl_FragColor= directional_light()*texture2D(u_texid, v_texcoord);
//gl_FragColor=texture2D(u_texid, v_texcoord);
//  gl_FragColor= directional_light();
}