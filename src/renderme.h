#ifndef RENDERME_H
#define RENDERME_H

#include <cglm/cglm.h>

#include "types.h"
#include "core/vec.h"
#include "material.h"
#include "wavefront.h"

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
    RDRME_DEBUG     = 0x1 << 5,
};

enum RendermeShaderType : u32 {
    SHADER_LIGHT_VN  = 0x0,
    SHADER_LIGHT_VNT = 0x1,
    SHADER_DEBUG_V   = 0x2
};

struct RenderMe {
    Material material;
    Transform transform;
    u32 vao;
    RendermeShaderType shader_type;

    // Defines rendering order for shader_data
    // Defaults to triangles if len = 0
    Vec<f32> shader_indices;

    // Data for shader attributes
    Vec<f32> shader_data;

    // The shader vertex count (n. times a vshader gets called)
    u32 shader_count;

    // Enables debug visualization for vertex positions
    b8 show_debug;

    // debug fragment color
    vec3 debug_color;

    // debug VAO
    u32 debug_VAO;

    // debug glDrawArrays count argument
    u32 debug_shader_count;
};

// This function right here should take care of which fragment shader to use
RenderMe rdrme_create(Vec<f32> data, RenderMeFlags flags, Material material);

// Setup debug points, instantiates a VBO and attaches a very simple shader program
// to render basic shape representing points in the object
void rdrme_setup_debug(RenderMe *renderme, Vec<f32> debug_points);

// Clones a renderme data exluding shader data
// Enables a somewhat acceptable instanticing behavior
void rdrme_clone(RenderMe *cloneme, RenderMe *dest);

// Creates a renderme object from an obj model data
RenderMe rdrme_from_obj(OBJModel *model, Material material,
        b8 gen_normals, b8 interp_normals);

#endif // RENDERME_H
