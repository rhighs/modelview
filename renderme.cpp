#include <memory.h>

#include "renderme.h"
#include "array.h"
#include "cglm/vec3.h"
#include "io.h"

#include "mesh_utils.h"
#include "wavefront.h"

/* If using gl3.h */
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#include <glad/glad.h>

#include  "shader.h"

static
Array<f32> zip_v_vn_tex(Array<f32> vertices, Array<f32> normals, Array<f32> texcoords) {
    assert(vertices.len == normals.len && texcoords.len/2 == vertices.len/3);

    Array<f32> result;
    const u32 len = texcoords.len + normals.len + vertices.len;
    array_init(&result, len);

    u32 vi=0, vni=0, texi=0;
    for (;result.len<len;) {
        array_push(&result, vertices[vi++]); array_push(&result, vertices[vi++]); array_push(&result, vertices[vi++]);
        array_push(&result, normals[vni++]); array_push(&result, normals[vni++]); array_push(&result, normals[vni++]);
        array_push(&result, texcoords[texi++]); array_push(&result, texcoords[texi++]);
    }

    return result;
}

static
Array<f32> zip_v_vn(Array<f32> vertices, Array<f32> normals) {
    assert(vertices.len == normals.len);

    Array<f32> result;
    const u32 len = normals.len + vertices.len;
    array_init(&result, len);

    u32 vi=0, vni=0;
    for (;result.len<len;) {
        array_push(&result, vertices[vi++]); array_push(&result, vertices[vi++]); array_push(&result, vertices[vi++]);
        array_push(&result, normals[vni++]); array_push(&result, normals[vni++]); array_push(&result, normals[vni++]);
    }

    return result;
}

static
Array<f32> zip_v_tex(Array<f32> vertices, Array<f32> texcoords) {
    assert(texcoords.len/2 == vertices.len/3);

    Array<f32> result;
    const u32 len = texcoords.len + vertices.len;
    array_init(&result, len);

    u32 vi=0, texi=0;
    for (;result.len<len;) {
        array_push(&result, vertices[vi++]); array_push(&result, vertices[vi++]); array_push(&result, vertices[vi++]);
        array_push(&result, texcoords[texi++]); array_push(&result, texcoords[texi++]);
    }

    return result;
}

f32 __debug_cube_vertices[] = {
    // Back quad
    -0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // Front quad
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // Left side quad
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    // Right side quad
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,

    // Bottom quad
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // Top quad
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
};


u32 __bind_texture_info(LoadedImage image) {
    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        image.width, image.height,
        0,
        GL_RGB, GL_UNSIGNED_BYTE, image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    return texture;
}

u32 __norm_tex_vao(Array<f32> data) {
    u32 VAO;
    u32 VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * data.len, data.data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2);

    return VAO;
}

u32 __norm_vao(Array<f32> data) {
    u32 VAO;
    u32 VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * data.len, data.data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);

    return VAO;
}

u32 __debug_vao(Array<f32> data) {
    u32 VAO;
    u32 VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * data.len, data.data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

#define DEBUG_SCALE 0.2f
Array<f32> __create_debug_vertices(Array<f32> debug_positions, Array<f32> debug_cube_verts) {
    Array<f32> result;
    array_init(&result, 32);

    for (u32 i=0; i<debug_positions.len-2; i+=3) {
        for (u32 j=0; j<debug_cube_verts.len-2; j+=3) {
            vec3 v = {debug_cube_verts[j+0],
                           debug_cube_verts[j+1],
                           debug_cube_verts[j+2]};
            glm_vec3_scale(v, DEBUG_SCALE, v);
            glm_vec3_add(v, (vec3){debug_positions[i+0], debug_positions[i+1], debug_positions[i+2]}, v);
            array_push(&result, v[0]);
            array_push(&result, v[1]);
            array_push(&result, v[2]);
        }
    }

    return result;
}
#undef DEBUG_SCALE

void rdrme_setup_debug(RenderMe *renderme, Array<f32> debug_points) {
    renderme->show_debug = TRUE;

    renderme->debug_color[0] = 0.0f;
    renderme->debug_color[1] = 0.0f;
    renderme->debug_color[2] = 1.0f;

    Array<f32> debug_verts = __create_debug_vertices(
        debug_points, 
        array_from(
            __debug_cube_vertices,
            RAW_ARRAY_LEN(__debug_cube_vertices)
        )
    );
    renderme->debug_VAO = __debug_vao(debug_verts);
    renderme->debug_shader_count = debug_verts.len/3;

    array_free(&debug_verts);
}

#define CHECKFLAG(A, B) ((A & B)!=0)
RenderMe rdrme_create(Array<f32> data, RenderMeFlags flags, Material material) {
    RenderMe result;

    u32 VAO = 0;
    u32 DATA_LINE_LENGTH = 0;

#ifdef MDEBUG
    IO_LOG(stdout, 
            "rdrme creation flags:"
            "\n\tRDRME_LIGHT=%d, \n\tRDRME_TEXTURE=%d,"
            "\n\tRDRME_NORMAL=%d, \n\tRDRME_DEBUG=%d,"
            "\n\tRDRME_BLANK=%d, \n\tRDRME_NORMAL=%d",
            CHECKFLAG(flags, RDRME_LIGHT),\
            CHECKFLAG(flags, RDRME_TEXTURE),\
            CHECKFLAG(flags, RDRME_NORMAL),\
            CHECKFLAG(flags, RDRME_DEBUG),\
            CHECKFLAG(flags, RDRME_BLANK),\
            CHECKFLAG(flags, RDRME_MATERIAL));
#endif

    if (
          CHECKFLAG(flags, RDRME_LIGHT)
       && CHECKFLAG(flags, RDRME_TEXTURE)
       && CHECKFLAG(flags, RDRME_NORMAL)
       ) {
        // vertex + normals + texture coords
        DATA_LINE_LENGTH = 8;
        VAO = __norm_tex_vao(data);
        result.shader_type = SHADER_LIGHT_VNT;
        IO_LOG(stdout, "using shader = SHADER_LIGHT_VNT", NULL);
    } else if (
            CHECKFLAG(flags, RDRME_LIGHT)
         && CHECKFLAG(flags, RDRME_NORMAL)
       ) {
        // vertex + normals
        DATA_LINE_LENGTH = 6;
        VAO = __norm_vao(data);
        result.shader_type = SHADER_LIGHT_VN;
        IO_LOG(stdout, "using shader = SHADER_LIGHT_VN", NULL);
    } else {
        DATA_LINE_LENGTH = 0;
        VAO = 0;
        #ifdef MDEBUG
            IO_LOG(stderr, "DATA_LINE_LENGTH is unset! current value = %d", DATA_LINE_LENGTH);
        #endif
    }

#ifdef RDR_DEBUG
    IO_LOG(stdout, "mesh data in renderme =", NULL);
    array_print(&data);
#endif

    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, result.transform.rotation);
    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, result.transform.translation);
    glm_vec3_copy((vec3) { 1.0f, 1.0f, 1.0f }, result.transform.scale);

    // TODO: indices should be given from caller
    array_init(&result.shader_indices, 4);

    // FIXME: ownership is outside!
    printf("render data info: %p (len: %d)\n", data.data, data.len);
    result.shader_data = data;
    result.shader_count = data.len / DATA_LINE_LENGTH;

    result.material = material;
    result.vao = VAO;

    return result;
}
#undef CHECKFLAG

void rdrme_clone(RenderMe *cloneme, RenderMe *dest) {
    memcpy((void *)dest, (void *)cloneme, sizeof(RenderMe));
}

RenderMe rdrme_from_obj(OBJModel *model, Material material,
        b8 gen_normals, b8 interp_normals) {
    RenderMe result;
    b8 has_texture = model->tex_coords.len > 0;
    b8 has_normals = model->normals.len > 0;

    Array<f32> rendering_data;

    RenderMeFlags rdrme_creation_flags = 0x0;

    if (has_texture && has_normals) {
        auto vertices = wf_model_extract_vertices(model);
        auto texcoords = wf_model_extract_texcoords(model);

        Array<f32> normals; 
        if (gen_normals == TRUE) {
            normals = mu_gen_normals(vertices);
        } else {
            normals = wf_model_extract_normals(model);
        }

        if (interp_normals == TRUE) {
            auto indices = wf_model_extract_indices(model);
            mu_interpolate_normals(normals, indices);
            array_free(&indices);
        }

        rendering_data = zip_v_vn_tex(vertices, normals, texcoords);

        array_free(&vertices);
        array_free(&normals);
        array_free(&texcoords);

        rdrme_creation_flags = RDRME_LIGHT
            | RDRME_TEXTURE
            | RDRME_NORMAL;
    } else if (has_normals) {
        auto vertices = wf_model_extract_vertices(model);

        Array<f32> normals; 
        if (gen_normals == TRUE) {
            normals = mu_gen_normals(vertices);
        } else {
            normals = wf_model_extract_normals(model);
        }

        if (interp_normals == TRUE) {
            auto indices = wf_model_extract_indices(model);
            mu_interpolate_normals(normals, indices);
            array_free(&indices);
        }

        rendering_data = zip_v_vn(vertices, normals);

        array_free(&vertices);
        array_free(&normals);

        rdrme_creation_flags = RDRME_LIGHT
            | RDRME_NORMAL;
    } else if (has_texture) {
        auto vertices = wf_model_extract_vertices(model);
        auto texcoords = wf_model_extract_texcoords(model);

        Array<f32> normals; 
        b8 has_normals = gen_normals == TRUE || interp_normals == TRUE;
        if (gen_normals == TRUE || interp_normals == TRUE) {
            normals = mu_gen_normals(vertices);
            auto indices = wf_model_extract_indices(model);
            mu_interpolate_normals(normals, indices);
            rendering_data = zip_v_vn_tex(vertices, normals, texcoords);

            array_free(&indices);
        } else {
            rendering_data = zip_v_tex(vertices, texcoords);
        }

        array_free(&vertices);
        array_free(&texcoords);

        rdrme_creation_flags = RDRME_LIGHT
            | RDRME_TEXTURE
            | (has_normals ? RDRME_NORMAL : 0x0);
    } else {
        auto vertices = wf_model_extract_vertices(model);

        Array<f32> normals; 
        if (gen_normals == TRUE || interp_normals == TRUE) {
            normals = mu_gen_normals(vertices);
            auto indices = wf_model_extract_indices(model);
            mu_interpolate_normals(normals, indices);
            rendering_data = zip_v_vn(vertices, normals);

            array_free(&indices);
        } else {
            rendering_data = array_from_copy(vertices.data, vertices.len);
        }

        array_free(&vertices);
        array_free(&normals);

        rdrme_creation_flags = RDRME_LIGHT
            | (has_normals ? RDRME_NORMAL : 0x0);
    }

    result = rdrme_create(rendering_data, rdrme_creation_flags, material);
    return result;
}

