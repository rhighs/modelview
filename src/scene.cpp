#include "scene.h"

#include <glm/glm.hpp>

PointLight point_light_make(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
    PointLight l;
    l.att_constant = 1.0f;
    l.att_linear = 0.08f;
    l.att_quadratic = 0.032f;
    l.intensity = 1.0f;

    l.ambient = ambient;
    l.diffuse = diffuse;
    l.specular = specular;
    l.position = glm::vec4(position, 1.0f);

    return l;
}
 
DirectionalLight directional_light_make(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
    DirectionalLight l;
    l.intensity = 1.0f;
    l.ambient = ambient;
    l.diffuse = diffuse;
    l.specular = specular;
    l.direction = glm::vec4(direction, 1.0f);
    return l;
}

Scene scene_init() {
    Scene result;
    result.directional_lights = Vec<DirectionalLight>(4);
    result.point_lights = Vec<PointLight>(4);
    return result;
}

void scene_add_point_light(Scene *scene, PointLight point_light) {
    scene->point_lights.push_back(point_light);
}

void scene_add_directional_light(Scene *scene, DirectionalLight directional_light) {
    scene->directional_lights.push_back(directional_light);
}
