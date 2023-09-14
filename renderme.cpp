#include "renderme.h"
#include "array.h"
#include "io.h"

/* If using gl3.h */
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#include <glad/glad.h>

#include  "shader.h"

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
    u32 VAO;
    u32 VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * len, data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2);

    return VAO;
}

u32 __norm_vao(f32 *data, u32 len) {
    u32 VAO;
    u32 VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * len, data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);

    return VAO;
}

RenderMe rdrme_create(Array<f32> data, RenderMeFlags flags, Material material) {
    RenderMe result;

    u32 VAO;
    // Default: vertex + normals
    u32 DATA_LINE_LENGTH;
    if (flags & RDRME_TEXTURE) {
        // vertex + normals + texture coords
        DATA_LINE_LENGTH = 8;
        VAO = __norm_tex_vao(data.data, data.len);
        result.shader_type = SHADER_LIGHT_VNT;
    } else {
        // vertex + normals
        DATA_LINE_LENGTH = 6;
        VAO = __norm_vao(data.data, data.len);
        result.shader_type = SHADER_LIGHT_VN;
    }

    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, result.transform.rotation);
    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, result.transform.translation);
    glm_vec3_copy((vec3) { 1.0f, 1.0f, 1.0f }, result.transform.scale);

    array_init(&result.mesh.indices, 4);

    // FIXME: ownership is outside!
    result.mesh.vertices = data;
    result.mesh.vertex_count = data.len / DATA_LINE_LENGTH;

    result.material = material;
    result.vao = VAO;

    return result;
}
