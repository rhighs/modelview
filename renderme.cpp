#include "renderme.h"
#include "io.h"

/* If using gl3.h */
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#include <glad/glad.h>

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

u32 __norm_tex_vao(f32 *data, u32 len) {
    u32 chicken_VAO;
    u32 chicken_VBO;
    glGenBuffers(1, &chicken_VBO);
    glGenVertexArrays(1, &chicken_VAO);

    glBindVertexArray(chicken_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, chicken_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * len, data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2);

    return chicken_VAO;
}

RenderMe rdrme_create(Array<f32> data, RenderMeFlags flags, Material material) {
    RenderMe result;

    // FIXME: this should be way different
    const u32 VAO = __norm_tex_vao(data.data, data.len);

    // Default: vertex + normals
    u32 DATA_LINE_LENGTH = 6;
    if (flags & RDRME_TEXTURE) {
        // vertex + normals + texture coords
        DATA_LINE_LENGTH = 8;
    }

    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, result.transform.rotation);
    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, result.transform.translation);
    glm_vec3_copy((vec3) { 1.0f, 1.0f, 1.0f }, result.transform.scale);

    result.mesh.indices = NULL;
    result.mesh.vertices = data.data;
    result.mesh.vertex_count = data.len / DATA_LINE_LENGTH;

    result.material = material;
    result.vao = VAO;

    return result;
}