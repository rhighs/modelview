#ifndef MATERIAL_H
#define MATERIAL_H

#include <cglm/cglm.h>

typedef struct {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} Material;

#endif // MATERIAL_H
