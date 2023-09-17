#ifndef LIGHT_H
#define LIGHT_H

#include "types.h"

#include <cglm/vec3.h>
#include <cglm/vec4.h>

#include "array.h"

struct PointLight {
    vec4 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    f32 att_constant;
    f32 att_linear;
    f32 att_quadratic;

    f32 intensity;
};

struct DirectionalLight {
    vec4 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    f32 intensity;
};

struct Scene {
    Array<PointLight> point_lights;
    Array<DirectionalLight> directional_lights;
};

PointLight pt_light_make(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular);

DirectionalLight dir_light_make(vec3 direction, vec3 ambient, vec3 diffuse, vec3 specular);

Scene scene_init();

void scene_add_point_light(Scene *scene, PointLight point_light);

void scene_add_directional_light(Scene *scene, DirectionalLight directional_light);

PointLight point_light_make(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular);
 
DirectionalLight directional_light_make(vec3 direction, vec3 ambient, vec3 diffuse, vec3 specular);

#endif // LIGHT_H
