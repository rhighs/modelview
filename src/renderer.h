#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>

#include "core/types.h"
#include "camera.h"
#include "shader.h"
#include "scene.h"
#include "material.h"
#include "renderme.h"

struct Renderer {
    Camera *camera;
    u32 vp_width;
    u32 vp_height;
};

Renderer rdr_init(Camera *camera, u32 width, u32 height);

void rdr_draw(Renderer *renderer, Scene *scene, RenderMe *renderme);

void rdr_clear_color(const glm::vec4 *color);

#endif // RENDERER_H
