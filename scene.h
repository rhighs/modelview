#ifndef LIGHT_H
#define LIGHT_H

#include "types.h"

#include <cglm/vec3.h>
#include <cglm/vec4.h>

typedef struct {
    vec4 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    f32 att_constant;
    f32 att_linear;
    f32 att_quadratic;
} PointLight;

typedef struct {
    vec4 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} DirectionalLight;

typedef struct {
    PointLight **pt_lights;
    DirectionalLight *dir_light;
} Scene;

PointLight pt_light_make(vec4 position, vec3 ambient, vec3 diffuse, vec3 specular);

#endif // LIGHT_H
