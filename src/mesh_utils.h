#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include <cglm/cglm.h>
#include "core/vec.h"
#include "core/types.h"

Vec<f32> mu_gen_normals(const Vec<f32>& perface_vertices);

void mu_interpolate_normals(Vec<f32>& perface_normals, Vec<u32>& perface_indices);

#endif // MESH_UTILS_H
