#include <cglm/cglm.h>
#include <glad/glad.h>

#include "array.h"
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

f32 __debug_cube_vertices[] = {
    // Back quad
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    // Front quad
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    // Left side quad
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,

    // Right side quad
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    // Bottom quad
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,

    // Top quad
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
};
    
RenderMe __debug_box;
Material __debug_material;

// ========================================================

Renderer rdr_init(Camera *camera, u32 width, u32 height) {
    Renderer result;

    {
        __debug_material = mat_make(MAT_CHROME, (vec3) { 0.0f, 0.0f, 1.0f });
        __debug_box = rdrme_create(
            array_from(__debug_cube_vertices, RAW_ARRAY_LEN(__debug_cube_vertices)),
            RDRME_LIGHT | RDRME_NORMAL,
            __debug_material
            );
    }
    
    if (!shaders_loaded) {
        light_tex_program = sp_create("./shaders/vert_norm_tex_v.glsl", "./shaders/texture_norm_light_f.glsl");
        light_program = sp_create("./shaders/vert_norm_tex_v.glsl", "./shaders/material_norm_light_f.glsl");
        shaders_loaded = TRUE;
    }

    result.camera = camera;
    result.vp_width = width;
    result.vp_height = height;

    return result;
}

void __rdr_draw(Renderer *renderer, Scene *scene, RenderMe *renderme) {
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
    glm_perspective(glm_rad(renderer->camera->view_angle_deg),
            (f32)renderer->vp_width/(f32)renderer->vp_height,
            renderer->camera->z_near,
            renderer->camera->z_far,
            projection);

    Transform *transform = &renderme->transform;
    mat4 model = {};
    glm_mat4_identity(model);
    glm_translate(model, transform->translation);
    glm_rotate(model, glm_rad(transform->rotation[0]), (vec3) { 1.0f, 0.0f, 0.0f });
    glm_rotate(model, glm_rad(transform->rotation[1]), (vec3) { 0.0f, 1.0f, 0.0f });
    glm_rotate(model, glm_rad(transform->rotation[2]), (vec3) { 0.0f, 0.0f, 1.0f });
    glm_scale(model, transform->scale);

    // Set material uniforms
    {
        auto m = renderme->material;
        sp_set_uniform_vec3f(program, "material.ambient",    m.ambient);
        sp_set_uniform_vec3f(program, "material.diffuse",    m.diffuse);
        sp_set_uniform_vec3f(program, "material.specular",   m.specular);
        sp_set_uniform_float(program, "material.shininess",  m.shininess);
    }

    // Set transformation matrices
    {
        sp_set_uniform_mat4(program, "model", model);
        sp_set_uniform_mat4(program, "view", view);
        sp_set_uniform_mat4(program, "projection", projection);
    }

    if (scene->point_lights.len > 0) {
        for (u32 i=0; i<scene->directional_lights.len; i++) {
            auto pl = scene->point_lights[i];

#ifdef RDR_DEBUG
        IO_LOG(stdout, "using point light n = %d (%f, %f, %f)", i,
                pl.position[0], pl.position[1], pl.position[2]);
#endif

            sp_set_uniform_vec3f(program, "point_lights[0].ambient",     pl.ambient);
            sp_set_uniform_vec3f(program, "point_lights[0].diffuse",     pl.diffuse);
            sp_set_uniform_vec3f(program, "point_lights[0].specular",    pl.specular);
            sp_set_uniform_vec4f(program, "point_lights[0].position",    pl.position);
            sp_set_uniform_float(program, "point_lights[0].constant",    pl.att_constant);
            sp_set_uniform_float(program, "point_lights[0].linear",      pl.att_linear);
            sp_set_uniform_float(program, "point_lights[0].quadratic",   pl.att_quadratic);
            sp_set_uniform_float(program, "point_lights[0].intensity",   pl.intensity);
        }
    }

    if (scene->directional_lights.len > 0) {
        for (u32 i=0; i<scene->directional_lights.len; i++) {
            auto dl = scene->directional_lights[0];
#ifdef RDR_DEBUG
        IO_LOG(stdout, "using directional light n = %d (%f, %f, %f)", i,
                dl.direction[0], dl.direction[1], dl.direction[2]);
#endif
            sp_set_uniform_vec3f(program, "dir_light.ambient",     dl.ambient);
            sp_set_uniform_vec3f(program, "dir_light.diffuse",     dl.diffuse);
            sp_set_uniform_vec3f(program, "dir_light.specular",    dl.specular);
            sp_set_uniform_vec4f(program, "dir_light.direction",   dl.direction);
            sp_set_uniform_float(program, "dir_light.intensity",   dl.intensity);
        }
    }

    sp_set_uniform_vec3f(program, "eye.position", renderer->camera->pos);

    glBindVertexArray(renderme->vao);
    if (renderme->shader_indices.len == 0) {
        // Vertex count is to be intented as the "attribute" unit or whatever.
        // As a question: how many "vertex shader" calls there will be? vertex_count shader calls
 #ifdef RDR_DEBUG
        fprintf(stdout, "[%s:%s:%d]: rendering vertex count = %d\n", __FILE__, __FUNCTION__, __LINE__, renderme->shader_count);
 #endif
        glDrawArrays(GL_TRIANGLES, 0, renderme->shader_count);
    }
}

void rdr_draw(Renderer *renderer, Scene *scene, RenderMe *renderme) {
    if (renderme->debug_draw) {
        const f32 scale_coeff = 0.02;
        __debug_box.transform.scale[0] = scale_coeff;
        __debug_box.transform.scale[1] = scale_coeff;
        __debug_box.transform.scale[2] = scale_coeff;
        // TEMP: debug draw object vertices
        for (u32 i=0; i<renderme->debug_points.len-3; i+=3) {
            glm_vec3_copy((vec3) {
                    renderme->debug_points[i+0] * renderme->transform.scale[0],
                    renderme->debug_points[i+1] * renderme->transform.scale[1],
                    renderme->debug_points[i+2] * renderme->transform.scale[2]
                    },
                    __debug_box.transform.translation);
            glm_vec3_rotate(__debug_box.transform.translation, glm_rad(renderme->transform.rotation[2]), (vec3){ 0.0f, 0.0f, 1.0f });
            glm_vec3_rotate(__debug_box.transform.translation, glm_rad(renderme->transform.rotation[1]), (vec3){ 0.0f, 1.0f, 0.0f });
            glm_vec3_rotate(__debug_box.transform.translation, glm_rad(renderme->transform.rotation[0]), (vec3){ 1.0f, 0.0f, 0.0f });
            glm_vec3_add(__debug_box.transform.translation, renderme->transform.translation, __debug_box.transform.translation);
            glm_vec3_copy(renderme->transform.rotation, __debug_box.transform.rotation);
            __rdr_draw(renderer, scene, &__debug_box);
        }
    }

    __rdr_draw(renderer, scene, renderme);
}

