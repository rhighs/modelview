#version 330 core

struct Material {
    // material colors
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

struct PointLight {
    vec4 position;

    // light colors
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Attenuation
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec4 direction;

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
uniform Eye eye;

#define N_POINT_LIGHTS 4
uniform PointLight point_lights[N_POINT_LIGHTS];
uniform DirectionalLight dir_light;

in vec3 normal;
in vec3 frag_pos;

float mat_ambient_intensity(PointLight light, vec3 diffuse, vec3 ambient) {
    return dot(light.ambient, ambient)/dot(light.ambient, diffuse);
}

float mat_ambient_intensity(DirectionalLight light, vec3 diffuse, vec3 ambient) {
    return dot(light.ambient, ambient)/dot(light.ambient, diffuse);
}

vec3 compute_dir_light(DirectionalLight light, vec3 normal) {
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    vec3 light_direction = -normalize(vec3(light.direction));

    // diffuse
    vec3 diffuse = max(dot(normal, light_direction), 0.0f) * light.diffuse;

    // spec
    vec3 view_direction = normalize(eye.position - frag_pos);
    vec3 reflect_direction = reflect(-light_direction, normal);

    // http://devernay.free.fr/cours/opengl/materials.html
    float spec_strength = pow(max(dot(view_direction, reflect_direction), 0.0f), 128.0 * material.shininess);
    vec3 specular = light.specular * (spec_strength * material.specular);

    return ambient + specular + diffuse;
}

float compute_light_attenuation(PointLight light) {
    float d = length(vec3(light.position) - frag_pos);
    float result = 1.0/(light.constant + light.linear*d + light.quadratic*d*d);
    return result;
}

vec3 compute_point_light(PointLight light, vec3 normal) {
    // ambient
    vec3 ambient = light.ambient * material.ambient * 2.0;

    // diffuse
    vec3 light_direction = normalize(vec3(light.position) - frag_pos);
    float diff_strength = max(dot(normal, light_direction), 0.0f);
    vec3 diffuse = (light.diffuse * material.diffuse) * diff_strength;

    // spec
    vec3 view_direction = normalize(eye.position - frag_pos);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec_strength = pow(max(dot(view_direction, reflect_direction), 0.0f), material.shininess * 128.0);
    vec3 specular = (light.specular * material.specular) * spec_strength;

    float attenuation = compute_light_attenuation(light);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main() {
    vec3 norm = normalize(normal);
    vec3 result = vec3(0.0);

    for (int i=0; i<N_POINT_LIGHTS; i++) {
        PointLight l = point_lights[i];
        if (l.position.w > 0.0) {
            result += compute_point_light(point_lights[i], norm);
        }
    }

    if (dir_light.direction.w > 0.0) {
        result += compute_dir_light(dir_light, norm);
    }

    vec4 texture_color = texture2D(tex, tex_coords);
    FragColor = texture_color * vec4(result, 1.0);
}

