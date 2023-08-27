#ifndef RENDERER_H
#define RENDERER_H

#include <cglm/cglm.h>

#include "types.h"
#include "camera.h"
#include "shader.h"
#include "scene.h"
#include "material.h"

typedef struct {
    u32 vertex_count;
    u32 *indices;
    f32 *vertices;
} Mesh;

typedef struct {
    vec3 scale;
    vec3 rotation;
    vec3 translation;
} Transform;

typedef struct {
    Material *material;
    Transform *transform;
    Mesh *mesh;
    u32 vao;
} RenderMe;

typedef struct {
    Camera *camera;
    ShaderProgram *program;
    u32 vp_width;
    u32 vp_height;
} Renderer;

void rdr_draw(Renderer *renderer, Scene *scene, RenderMe *renderme);

#endif // RENDERER_H
