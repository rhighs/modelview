#include "material.h"

Material mat_make(Material base_material, glm::vec3 color) {
    Material mat = {};

    mat.ambient = base_material.ambient * color;
    mat.diffuse = base_material.diffuse * color;
    mat.specular = base_material.specular * color;
    mat.shininess = base_material.shininess;

    return mat;
}
