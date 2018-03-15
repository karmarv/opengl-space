#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

out vec4 v_position;
out vec3 v_normal;

uniform mat4 u_transformationMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_viewMatrix;
uniform mat3 u_normalMatrix;

void main() {
    vec4 pos = u_transformationMatrix*position;
    v_position = pos;
	//Normal = mat3(transpose(inverse(model))) * normal;
    v_normal = normalize(u_normalMatrix*normal);
    gl_Position = u_projectionMatrix*u_viewMatrix*pos;
}
