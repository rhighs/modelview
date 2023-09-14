#include <cglm/cglm.h>
#include <glad/glad.h>

#include "renderer.h"
#include "material.h"
#include "shader.h"
#include "camera.h"
#include "scene.h"
#include "types.h"

#include "io.h"

#ifdef RDR_DEBUG
#include <stdio.h>
#endif

// private globals
// ========================================================
b8 shaders_loaded = FALSE;
ShaderProgram light_tex_program;
ShaderProgram light_program;
// ========================================================

Renderer rdr_init(Camera *camera, u32 width, u32 height) {
    Renderer result;
    
    if (!shaders_loaded) {
        light_tex_program = sp_create("./shaders/vert_norm_tex_v.glsl", "./shaders/texture_norm_light_f.glsl");
        light_program = sp_create("./shaders/vert_norm_v.glsl", "./shaders/material_norm_light_f.glsl");
        shaders_loaded = TRUE;
    }

    result.camera = camera;
    result.vp_width = width;
    result.vp_height = height;

    return result;
}

void rdr_draw(Renderer *renderer, Scene *scene, RenderMe *renderme) {
    ShaderProgram *program;
    switch (renderme->shader_type) {
        case SHADER_LIGHT_VN: 
            program = &light_program;
            break;
        case SHADER_LIGHT_VNT: 
            program = &light_tex_program;
            break;
    }
    sp_use(program);

    mat4 view = {};
    vec3 camera_target = {};
    glm_vec3_add(renderer->camera->pos, renderer->camera->front, camera_target);
    glm_lookat(renderer->camera->pos, camera_target, renderer->camera->up, view); 

    mat4 projection = {};
    glm_perspective(glm_rad(45.0f), (f32)renderer->vp_width/(f32)renderer->vp_height, 0.1f, 100.0f, projection);

    Transform *transform = &renderme->transform;
    mat4 model = {};
    glm_mat4_identity(model);
    glm_translate(model, transform->translation);
    glm_rotate(model, glm_rad(transform->rotation[0]), (vec3) { 1.0f, 0.0f, 0.0f });
    glm_rotate(model, glm_rad(transform->rotation[1]), (vec3) { 0.0f, 1.0f, 0.0f });
    glm_rotate(model, glm_rad(transform->rotation[2]), (vec3) { 0.0f, 0.0f, 1.0f });
    glm_scale(model, transform->scale);

    sp_set_uniform_vec3f(program, "material.ambient",    renderme->material.ambient);
    sp_set_uniform_vec3f(program, "material.diffuse",    renderme->material.diffuse);
    sp_set_uniform_vec3f(program, "material.specular",   renderme->material.specular);
    sp_set_uniform_float(program, "material.shininess",  renderme->material.shininess);

    sp_set_uniform_mat4(program, "model", model);
    sp_set_uniform_mat4(program, "view", view);
    sp_set_uniform_mat4(program, "projection", projection);

    if (scene->point_lights.len > 0) {
        sp_set_uniform_vec3f(program, "point_lights[0].ambient",     scene->point_lights[0].ambient);
        sp_set_uniform_vec3f(program, "point_lights[0].diffuse",     scene->point_lights[0].diffuse);
        sp_set_uniform_vec3f(program, "point_lights[0].specular",    scene->point_lights[0].specular);
        sp_set_uniform_vec4f(program, "point_lights[0].position",    scene->point_lights[0].position);
        sp_set_uniform_float(program, "point_lights[0].constant",    scene->point_lights[0].att_constant);
        sp_set_uniform_float(program, "point_lights[0].linear",      scene->point_lights[0].att_linear);
        sp_set_uniform_float(program, "point_lights[0].quadratic",   scene->point_lights[0].att_quadratic);
    }

    if (scene->directional_lights.len > 0) {
        sp_set_uniform_vec3f(program, "dir_light.ambient",     scene->directional_lights[0].ambient);
        sp_set_uniform_vec3f(program, "dir_light.diffuse",     scene->directional_lights[0].diffuse);
        sp_set_uniform_vec3f(program, "dir_light.specular",    scene->directional_lights[0].specular);
        sp_set_uniform_vec4f(program, "dir_light.direction",   scene->directional_lights[0].direction);
    }

    sp_set_uniform_vec3f(program, "eye.position", renderer->camera->pos);

    glBindVertexArray(renderme->vao);
    if (renderme->mesh.indices.len == 0) {
 #ifdef RDR_DEBUG
        fprintf(stdout, "[%s:%s:%d]: rendering vertex count = %d\n", __FILE__, __FUNCTION__, __LINE__, renderme->mesh->vertex_count);
 #endif
        glDrawArrays(GL_TRIANGLES, 0, renderme->mesh.vertex_count);
    }
}

