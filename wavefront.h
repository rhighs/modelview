#ifndef WAVEFRONT_H
#define WAVEFRONT_H

#include "types.h"
#include "array.h"

struct OBJFaceVertex {
    u32 vertex_id;
    u32 tex_coord_id;
    u32 normal_id;
};

struct OBJModel {
    Array<f32> vertices;
    Array<f32> normals;
    Array<f32> tex_coords;
    Array<u32> indices;
    Array<Array<OBJFaceVertex>> faces;
};

Array<f32> wf_model_zip_v_vn_tex(OBJModel *model);

Array<f32> wf_model_zip_v_vn(OBJModel *model);

OBJModel wf_load_obj_model(const char *path);

Array<u32> wf_model_extract_indices(OBJModel *model);

Array<f32> wf_model_extract_vertices(OBJModel *model);

Array<f32> wf_model_extract_texcoords(OBJModel *model);

Array<f32> wf_model_extract_normals(OBJModel *model);

#endif // WAVEFRONT_H
