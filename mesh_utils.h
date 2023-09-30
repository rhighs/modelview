#ifndef MESH_UTILS_H
#define MESH_UTILS_H

#include <cglm/cglm.h>
#include "array.h"
#include "types.h"

Array<f32> mu_gen_normals(Array<f32> perface_vertices);

void mu_interpolate_normals(Array<f32> perface_normals, Array<u32> perface_indices);

#endif // MESH_UTILS_H
