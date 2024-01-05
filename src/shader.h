#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "core/types.h"

struct ShaderProgram {
    u32 program;
};

void sp_use(ShaderProgram *program);

void sp_set_uniform_vec3f(ShaderProgram *program, const char* uniform,
        glm::vec3 v);

void sp_set_uniform_vec4f(ShaderProgram *program, const char* uniform,
        glm::vec4 v);

void sp_set_uniform_mat4(ShaderProgram *program, const char* uniform,
        const glm::mat4 &mat);

void sp_set_uniform_float(ShaderProgram *program, const char* uniform,
        const f32 value);

ShaderProgram sp_create(
        const char *vert_source_path,
        const char *frag_source_path
        );

#endif // SHADER_H
