#include "scene.h"
#include "cglm/vec4.h"

#include <cglm/cglm.h>

PointLight point_light_make(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular) {
    PointLight l;
    l.att_constant = 1.0f;
    l.att_linear = 0.08f;
    l.att_quadratic = 0.032f;
    l.intensity = 1.0f;

    glm_vec4_copy(vec4 { position[0], position[1], position[2], 1.0f }, l.position);
    glm_vec3_copy(ambient, l.ambient);
    glm_vec3_copy(diffuse, l.diffuse);
    glm_vec3_copy(specular, l.specular);

    return l;
}
 
DirectionalLight directional_light_make(vec3 direction, vec3 ambient, vec3 diffuse, vec3 specular) {
    DirectionalLight l;
    l.intensity = 1.0f;
    glm_vec4_copy(vec4 { direction[0], direction[1], direction[2], 1.0f }, l.direction);
    glm_vec3_copy(ambient, l.ambient);
    glm_vec3_copy(diffuse, l.diffuse);
    glm_vec3_copy(specular, l.specular);
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
