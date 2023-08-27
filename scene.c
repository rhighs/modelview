#include "scene.h"
#include "cglm/vec4.h"

#include <cglm/cglm.h>

PointLight pt_light_make(vec4 position, vec3 ambient, vec3 diffuse, vec3 specular) {
    PointLight l = {
        .ambient = {},
        .diffuse= {},
        .specular = {},
        .att_constant = 1.0f,
        .att_linear = 0.08f,
        .att_quadratic = 0.032f
    };

    glm_vec4_copy(position, l.position);
    glm_vec3_copy(ambient, l.ambient);
    glm_vec3_copy(diffuse, l.diffuse);
    glm_vec3_copy(specular, l.specular);

    return l;
}
