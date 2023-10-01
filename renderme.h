#ifndef RENDERME_H
#define RENDERME_H

#include <cglm/cglm.h>

#include "types.h"
#include "array.h"
#include "material.h"

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

enum RendermeShaderType : u32 {
    SHADER_LIGHT_VN  = 0x0,
    SHADER_LIGHT_VNT = 0x1,
};

struct RenderMe {
    Material material;
    Transform transform;
    u32 vao;
    RendermeShaderType shader_type;

    // Defines rendering order for shader_data
    // Defaults to triangles if len = 0
    Array<f32> shader_indices;

    // Data for shader attributes
    Array<f32> shader_data;

    // The shader vertex count (n. times a vshader gets called)
    u32 shader_count;

    // Enables debug visualization for vertex positions
    b8 debug_draw;
    Array<f32> debug_points;
};

// This function right here should take care of which fragment shader to use
RenderMe rdrme_create(Array<f32> data, RenderMeFlags flags, Material material);

#endif // RENDERME_H
