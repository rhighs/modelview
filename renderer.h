#ifndef RENDERER_H
#define RENDERER_H

#include <cglm/cglm.h>

#include "types.h"
#include "camera.h"
#include "shader.h"
#include "scene.h"
#include "material.h"
#include "renderme.h"

struct Renderer {
    Camera *camera;
    ShaderProgram *program;
    u32 vp_width;
    u32 vp_height;
};

void rdr_draw(Renderer *renderer, Scene *scene, RenderMe *renderme);

#endif // RENDERER_H
