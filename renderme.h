#ifndef RENDERME_H
#define RENDERME_H

#include <cglm/cglm.h>

#include "types.h"
#include "array.h"
#include "material.h"

struct Mesh {
    u32 vertex_count;
    Array<u32> indices;
    Array<f32> vertices;
};

struct Transform {
    vec3 scale;
    vec3 rotation;
    vec3 translation;
};

typedef u32 RenderMeFlags;
enum _RenderMeFlags {
    RDRME_LIGHT     = 0x1 << 0,
    RDRME_MATERIAL  = 0x1 << 1,
    RDRME_TEXTURE   = 0x1 << 2,
    RDRME_NORMAL    = 0x1 << 3,
    RDRME_BLANK     = 0x1 << 4,
};

struct RenderMe {
    Material material;
    Transform transform;
    Mesh mesh;
    u32 vao;
};

// Functions here should take care of which fragment shader to use
RenderMe rdrme_create(Array<f32> data, RenderMeFlags flags, Material material);

#endif // RENDERME_H
