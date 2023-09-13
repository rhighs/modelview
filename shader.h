#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <cglm/cglm.h>

#include "types.h"

struct ShaderProgram {
    u32 program;
    u32 VAO;
};

void sp_bind_vao(ShaderProgram *program, const u32 VAO);

void sp_use(ShaderProgram *program);

void sp_set_uniform_vec3f(ShaderProgram *program, const char* uniform,
        vec3 v);

void sp_set_uniform_vec4f(ShaderProgram *program, const char* uniform,
        vec4 v);

void sp_set_uniform_mat4(ShaderProgram *program, const char* uniform,
        const mat4 mat);

void sp_set_uniform_float(ShaderProgram *program, const char* uniform,
        const f32 value);

ShaderProgram sp_create(
        const char *vert_source_path,
        const char *frag_source_path
        );

#endif // SHADER_H
