#version 330 core

struct Material {
    // material colors
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

struct Light {
    vec3 position;

    // light colors
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Eye {
    vec3 position;
};

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform Eye eye;

in vec3 normal;
in vec3 frag_pos;

void main() {
    vec3 norm = normalize(normal);

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 light_direction = normalize(light.position - frag_pos);
    vec3 diffuse = max(dot(norm, light_direction), 0.0f) * light.diffuse;

    // spec
    vec3 view_direction = normalize(eye.position - frag_pos);
    vec3 reflect_direction = reflect(-light_direction, norm);
    float spec_strength = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess);
    vec3 specular = light.specular * (spec_strength * material.specular);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}

