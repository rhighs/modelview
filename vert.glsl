#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_pos;

out vec2 tex_coord;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(a_pos, 1.0);
    tex_coord = a_tex_pos;
}