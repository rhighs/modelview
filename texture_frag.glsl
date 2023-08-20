#version 330 core

in vec2 tex_coord;

out vec4 FragColor;

uniform sampler2D my_texture;

void main() {
    FragColor = texture(my_texture, tex_coord);
}
