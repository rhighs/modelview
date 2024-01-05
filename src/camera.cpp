#include <SDL2/SDL_scancode.h>

#include "camera.h"
#include "io.h"

void camera_init(Camera *camera, glm::vec3 pos) {
    camera->pos = pos;
    camera->up = { 0.0f, 1.0f, 0.0f }; 
    camera->target = { 0.0f, 0.0f, 0.0f };
    camera->front = { 0.0f, 0.0f, -1.0f };
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

    glm::vec3 camera_dir(
        glm::cos(glm::radians(camera->yaw)) * glm::cos(glm::radians(camera->pitch)),
        glm::sin(glm::radians(camera->pitch)),
        glm::sin(glm::radians(camera->yaw)) * glm::cos(glm::radians(camera->pitch))
    );

    camera->front = glm::normalize(camera_dir);
}

void camera_update(Camera *camera, f32 dt) {
    // Update camera position
    {
        if (io_is_key_pressed(SDL_SCANCODE_W)) {
            glm::vec3 camera_front = camera->front * (camera->speed * dt);
            camera->pos += camera_front;
        }
        if (io_is_key_pressed(SDL_SCANCODE_S)) {
            glm::vec3 camera_front = camera->front * (camera->speed * dt);
            camera->pos -= camera_front;
        }
        if (io_is_key_pressed(SDL_SCANCODE_D)) {
            glm::vec3 camera_right = glm::cross(camera->front, camera->up);
            camera_right *= camera->speed * dt;
            camera->pos += camera_right;
        }
        if (io_is_key_pressed(SDL_SCANCODE_A)) {
            glm::vec3 camera_right = glm::cross(camera->front, camera->up);
            camera_right *= camera->speed * dt;
            camera->pos -= camera_right;
        }
    }
}
