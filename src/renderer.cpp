#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "core/vec.h"
#include "renderer.h"
#include "material.h"
#include "renderme.h"
#include "shader.h"
#include "camera.h"
#include "scene.h"
#include "core/types.h"

#include "io.h"

#ifdef RDR_DEBUG
#include <stdio.h>
#endif

#include "load.h"

// private globals
// ========================================================
b8 shaders_loaded = FALSE;
ShaderProgram light_tex_program;
ShaderProgram light_program;
ShaderProgram debug_program;
    
Material __debug_material;
// ========================================================

Renderer rdr_init(Camera *camera, u32 width, u32 height) {
    Renderer result;
    {
        __debug_material = mat_make(MAT_WHITE_PLASTIC, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    
    if (!shaders_loaded) {
        String light_tex_program_v_path = Loader::resolve_filepath("./shaders/vert_norm_tex_v.glsl");
        String light_tex_program_f_path = Loader::resolve_filepath("./shaders/texture_norm_light_f.glsl");
        light_tex_program = sp_create(light_tex_program_v_path.raw(), light_tex_program_f_path.raw());
        
        String light_program_v_path = Loader::resolve_filepath("./shaders/vert_norm_tex_v.glsl");
        String light_program_f_path = Loader::resolve_filepath("./shaders/material_norm_light_f.glsl");
        light_program = sp_create(light_program_v_path.raw(), light_program_f_path.raw());
        
        String debug_program_v_path = Loader::resolve_filepath("./shaders/vert_debug_v.glsl");
        String debug_program_f_path = Loader::resolve_filepath("./shaders/debug_f.glsl");
        debug_program = sp_create(debug_program_v_path.raw(), debug_program_f_path.raw());

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
        case SHADER_DEBUG_V: 
            program = &debug_program;
            break;
        case SHADER_LIGHT_VN: 
            program = &light_program;
            break;
        case SHADER_LIGHT_VNT: 
            program = &light_tex_program;
            break;
    }
    sp_use(program);

    Camera *camera = renderer->camera;

    glm::vec3 camera_target = camera->pos + camera->front;
    glm::mat4 view = glm::lookAt(camera->pos, camera_target, camera->up);

    glm::mat4 projection = glm::perspective(glm::radians(camera->view_angle_deg),
            (f32)renderer->vp_width/(f32)renderer->vp_height,
            renderer->camera->z_near,
            renderer->camera->z_far);

    Transform *transform = &renderme->transform;
    glm::mat4 model(1.0f);
    model = glm::translate(model, transform->translation);
    model = glm::rotate(model, glm::radians(transform->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform->scale);

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

    if (scene->point_lights.len() > 0 && renderme->shader_type != SHADER_DEBUG_V) {
        for (u32 i=0; i<scene->directional_lights.len(); i++) {
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

    if (scene->directional_lights.len() > 0) {
        for (u32 i=0; i<scene->directional_lights.len(); i++) {
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
    if (renderme->shader_indices.len() == 0) {
        // Vertex count is to be intented as the "attribute" unit or whatever.
        // As a question: how many "vertex shader" calls will there be? vertex_count shader calls
 #ifdef RDR_DEBUG
        fprintf(stdout, "[%s:%s:%d]: rendering vertex count = %d\n", __FILE__, __FUNCTION__, __LINE__, renderme->shader_count);
 #endif
        glDrawArrays(GL_TRIANGLES, 0, renderme->shader_count);
    }
}

void rdr_draw(Renderer *renderer, Scene *scene, RenderMe *renderme) {
    if (renderme->show_debug) {
        ShaderProgram *program = &debug_program;
        sp_use(program);

        Camera *camera = renderer->camera;
        glm::vec3 camera_target = camera->pos + camera->front;
        glm::mat4 view = glm::lookAt(renderer->camera->pos, camera_target, renderer->camera->up); 

        glm::mat4 projection = glm::perspective(glm::radians(renderer->camera->view_angle_deg),
                (f32)renderer->vp_width/(f32)renderer->vp_height,
                camera->z_near,
                camera->z_far);

        Transform *transform = &renderme->transform;
        glm::mat4 model(1.0f);
        model = glm::translate(model, transform->translation);
        model = glm::rotate(model, glm::radians(transform->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(transform->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(transform->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, transform->scale);

        // Set debug color uniform
        {
            sp_set_uniform_vec3f(program, "color", renderme->debug_color);
        }

        // Set transformation matrices
        {
            sp_set_uniform_mat4(program, "model", model);
            sp_set_uniform_mat4(program, "view", view);
            sp_set_uniform_mat4(program, "projection", projection);
        }

        glBindVertexArray(renderme->debug_VAO);
        glDrawArrays(GL_TRIANGLES, 0, renderme->debug_shader_count);
    }

    __rdr_draw(renderer, scene, renderme);
}

void rdr_clear_color(const glm::vec4 *color) {
    glClearColor(
            (*color)[0],
            (*color)[1],
            (*color)[2],
            (*color)[3]);

    // depth buffer cleared here atm...
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
