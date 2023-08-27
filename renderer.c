#include <cglm/cglm.h>
#include <glad/glad.h>

#include "renderer.h"
#include "material.h"
#include "shader.h"
#include "camera.h"
#include "scene.h"

void rdr_draw(Renderer *renderer, Scene *scene, RenderMe *renderme) {
    sp_use(renderer->program);

    mat4 view = {};
    vec3 camera_target = {};
    glm_vec3_add(renderer->camera->pos, renderer->camera->front, camera_target);
    glm_lookat(renderer->camera->pos, camera_target, renderer->camera->up, view); 

    mat4 projection = {};
    glm_perspective(glm_rad(45.0f), (f32)renderer->vp_width/(f32)renderer->vp_height, 0.1f, 100.0f, projection);

    Transform *transform = renderme->transform;
    mat4 model = {};
    glm_mat4_identity(model);
    glm_translate(model, transform->translation);
    glm_rotate(model, glm_rad(transform->rotation[0]), (vec3) { 1.0f, 0.0f, 0.0f });
    glm_rotate(model, glm_rad(transform->rotation[1]), (vec3) { 0.0f, 1.0f, 0.0f });
    glm_rotate(model, glm_rad(transform->rotation[2]), (vec3) { 0.0f, 0.0f, 1.0f });
    glm_scale(model, transform->scale);

    sp_set_uniform_vec3f(renderer->program, "material.ambient",    renderme->material->ambient);
    sp_set_uniform_vec3f(renderer->program, "material.diffuse",    renderme->material->diffuse);
    sp_set_uniform_vec3f(renderer->program, "material.specular",   renderme->material->specular);
    sp_set_uniform_float(renderer->program, "material.shininess",  renderme->material->shininess);

    sp_set_uniform_mat4(renderer->program, "model", model);
    sp_set_uniform_mat4(renderer->program, "view", view);
    sp_set_uniform_mat4(renderer->program, "projection", projection);

    if (scene->pt_lights != NULL) {
        PointLight *lights = *(scene->pt_lights);
        sp_set_uniform_vec3f(renderer->program, "point_lights[0].ambient",     lights[0].ambient);
        sp_set_uniform_vec3f(renderer->program, "point_lights[0].diffuse",     lights[0].diffuse);
        sp_set_uniform_vec3f(renderer->program, "point_lights[0].specular",    lights[0].specular);
        sp_set_uniform_vec4f(renderer->program, "point_lights[0].position",    lights[0].position);
        sp_set_uniform_float(renderer->program, "point_lights[0].constant",    lights[0].att_constant);
        sp_set_uniform_float(renderer->program, "point_lights[0].linear",      lights[0].att_linear);
        sp_set_uniform_float(renderer->program, "point_lights[0].quadratic",   lights[0].att_quadratic);
    }

    if (scene->dir_light != NULL) {
        DirectionalLight dir_light = *(scene->dir_light);
        sp_set_uniform_vec3f(renderer->program, "dir_light.ambient",     dir_light.ambient);
        sp_set_uniform_vec3f(renderer->program, "dir_light.diffuse",     dir_light.diffuse);
        sp_set_uniform_vec3f(renderer->program, "dir_light.specular",    dir_light.specular);
        sp_set_uniform_vec4f(renderer->program, "dir_light.direction",   dir_light.direction);
    }

    sp_set_uniform_vec3f(renderer->program, "eye.position", renderer->camera->pos);

    glBindVertexArray(renderme->vao);
    if (renderme->mesh->indices == NULL) {
        glDrawArrays(GL_TRIANGLES, 0, renderme->mesh->vertex_count);
    }
}

