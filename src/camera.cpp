#include <SDL2/SDL_scancode.h>
#include <cglm/util.h>
#include <cglm/vec3.h>

#include "camera.h"
#include "io.h"

void camera_init(Camera *camera, vec3 pos) {
    glm_vec3_copy(pos, camera->pos);
    glm_vec3_copy(vec3 { 0.0f, 1.0f, 0.0f }, camera->up);
    glm_vec3_copy(vec3 { 0.0f, 0.0f, 0.0f }, camera->target);
    glm_vec3_copy(vec3 { 0.0f, 0.0f, -1.0f }, camera->front);
    camera->sens = __CAMERA_DEFAULT_SENS;
    camera->speed = __CAMERA_DEFAULT_SPEED;
    camera->yaw = 0.0f;
    camera->pitch = 0.0f;
    camera->z_near = 0.1f;
    camera->z_far = 1000.0f;
    camera->view_angle_deg = 45.0f;
}

void camera_update_direction(Camera *camera, const f32 xrel, const f32 yrel) {
    const f32 mx = xrel * camera->sens;
    const f32 my = yrel * camera->sens * -1;
    camera->yaw   += mx;
    camera->pitch += my;
    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    vec3 camera_dir = { 
        cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
        sin(glm_rad(camera->pitch)),
        sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)),
    };

    glm_normalize_to(camera_dir, camera->front);
}

void camera_update(Camera *camera, f32 dt) {

    // Update camera position
    {
    if (io_is_key_pressed(SDL_SCANCODE_W)) {
        vec3 camera_front = {};
        glm_vec3_scale(camera->front, camera->speed * dt, camera_front);
        glm_vec3_add(camera->pos, camera_front, camera->pos);
    }
    if (io_is_key_pressed(SDL_SCANCODE_S)) {
        vec3 camera_front = {};
        glm_vec3_scale(camera->front, camera->speed * dt, camera_front);
        glm_vec3_sub(camera->pos, camera_front, camera->pos);
    }
    if (io_is_key_pressed(SDL_SCANCODE_D)) {
        vec3 camera_right = {};
        glm_cross(camera->front, camera->up, camera_right);
        glm_normalize(camera_right);
        glm_vec3_scale(camera_right, camera->speed * dt, camera_right);
        glm_vec3_add(camera->pos, camera_right, camera->pos);
    }
    if (io_is_key_pressed(SDL_SCANCODE_A)) {
        vec3 camera_right = {};
        glm_cross(camera->front, camera->up, camera_right);
        glm_normalize(camera_right);
        glm_vec3_scale(camera_right, camera->speed * dt, camera_right);
        glm_vec3_sub(camera->pos, camera_right, camera->pos);
    }
    }
}
