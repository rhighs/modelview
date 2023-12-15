#ifndef WAVEFRONT_H
#define WAVEFRONT_H

#include "core/types.h"
#include "core/vec.h"

struct OBJFaceVertex {
    u32 vertex_id;
    u32 tex_coord_id;
    u32 normal_id;
};

struct OBJModel {
    Vec<f32> vertices;
    Vec<f32> normals;
    Vec<f32> tex_coords;
    Vec<u32> indices;
    Vec<Vec<OBJFaceVertex>> faces;
};

Vec<f32> wf_model_zip_v_vn_tex(OBJModel *model);

Vec<f32> wf_model_zip_v_vn(OBJModel *model);

OBJModel wf_load_obj_model(const char *path);

Vec<u32> wf_model_extract_indices(OBJModel *model);

Vec<f32> wf_model_extract_vertices(OBJModel *model);

Vec<f32> wf_model_extract_texcoords(OBJModel *model);

Vec<f32> wf_model_extract_normals(OBJModel *model);

#endif // WAVEFRONT_H
