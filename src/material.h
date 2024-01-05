#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

#define MAT_CHROME (Material {\
        {0.25, 0.25, 0.25},\
        {0.4, 0.4, 0.4},\
        {0.774597, 0.774597, 0.774597},\
        0.6\
    })

#define MAT_WHITE_PLASTIC (Material {\
    {0.0, 0.0, 0.0},\
    {0.55, 0.55, 0.55},\
    {0.70, 0.70, 0.70},\
    0.25\
    })

#define MAT_WHITE_RUBBER (Material {\
    {0.05, 0.05, 0.05},\
    {0.5, 0.5, 0.5},\
    {0.7, 0.7, 0.7},\
    0.078125\
})

Material mat_make(Material base_material, glm::vec3 color);

#endif // MATERIAL_H

