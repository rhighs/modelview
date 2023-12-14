#include <glad/glad.h>
#include <cglm/cglm.h>

#include "shader.h"

static
char *read_shader_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    u32 filesize = ftell(file);
    rewind(file);

    char *shader_content = (char *)malloc(filesize+1);
    if (!fread(shader_content, sizeof(char), filesize, file)) {
        fprintf(stderr, "Could not read shader file: %s\n", filepath);
        exit(-1);
    }
    shader_content[filesize] = '\0';

    return shader_content;
}

void sp_use(ShaderProgram *program) {
    glUseProgram(program->program);
}

void sp_set_uniform_vec3f(ShaderProgram *program, const char* uniform,
        vec3 v) {
    const i32 uniform_location = glGetUniformLocation(program->program, uniform);
    glUniform3f(uniform_location, v[0], v[1], v[2]);
}

void sp_set_uniform_vec4f(ShaderProgram *program, const char* uniform,
        vec4 v) {
    const i32 uniform_location = glGetUniformLocation(program->program, uniform);
    glUniform4f(uniform_location, v[0], v[1], v[2], v[3]);
}

void sp_set_uniform_mat4(ShaderProgram *program, const char* uniform,
        const mat4 mat) {
    const i32 uniform_location = glGetUniformLocation(program->program, uniform);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, (float *)mat);
}

void sp_set_uniform_float(ShaderProgram *program, const char* uniform,
        const f32 value) {
    const i32 uniform_location = glGetUniformLocation(program->program, uniform);
    glUniform1f(uniform_location, value);
}

ShaderProgram sp_create(
        const char *vert_source_path,
        const char *frag_source_path
        ) {
    i32 success;
    char info_log[512];

    const char *vertex_shader_content = read_shader_file(vert_source_path);
    const u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_content, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        fprintf(stderr, "[SHADER_C_ERR] %s: %s\n\n",
                vert_source_path, info_log);
    }

    const char *fragment_shader_content = read_shader_file(frag_source_path);
    const u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_content, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        fprintf(stderr, "[SHADER_C_ERR] %s: %s\n\n",
                frag_source_path, info_log);
    }

    const u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        fprintf(stderr, "[PROGRAM_LINK_ERR]: %s\n", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return (ShaderProgram) { .program = shader_program };
}
