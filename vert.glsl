#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_norm;

out vec3 normal;
out vec3 frag_pos;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * transform * vec4(a_pos, 1.0);
    frag_pos = vec3(transform * vec4(a_pos, 1.0f));
    normal = mat3(transpose(inverse(transform))) * a_norm;
}
