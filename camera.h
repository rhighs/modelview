#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

#include "types.h"

#define __CAMERA_DEFAULT_SPEED 10.0f
#define __CAMERA_DEFAULT_SENS 0.1f

struct Camera {
    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 target;
    f32 speed;
    f32 sens;
    f32 yaw;
    f32 pitch;
    f32 z_far;
    f32 z_near;
    f32 view_angle_deg;
};

void camera_init(Camera *camera, vec3 pos);

void camera_update_direction(Camera *camera, const f32 xrel, const f32 yrel);

void camera_update(Camera *camera, f32 dt);

#endif // CAMERA_H
