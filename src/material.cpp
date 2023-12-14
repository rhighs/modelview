#include "material.h"

Material mat_make(Material base_material, vec3 color) {
    Material mat = {};

    glm_vec3_mul(base_material.ambient, color, mat.ambient);
    glm_vec3_mul(base_material.diffuse, color, mat.diffuse);
    glm_vec3_mul(base_material.specular, color, mat.specular);
    mat.shininess = base_material.shininess;

    return mat;
}
