#ifndef RENDERME_H
#define RENDERME_H

#include "types.h"
#include "array.h"
#include "renderer.h"

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