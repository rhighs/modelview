#ifndef WAVEFRONT_H
#define WAVEFRONT_H

#include "core/string.h"
#include "core/types.h"
#include "core/vec.h"

struct OBJFaceVertex {
    u32 vertex_id = 0;
    u32 tex_coord_id = 0;
    u32 normal_id = 0;
};

struct OBJMaterial {
    String name;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // 1 = flat material with no specular highlights, 2 = specular highlights present
    u8 illum_state = 1;
    f32 shininess;
    f32 transparency;

    String texture_filepath;

    String print() const;
};

struct OBJModel {
    Vec<f32> vertices;
    Vec<f32> normals;
    Vec<f32> tex_coords;
    Vec<u32> indices;
    Vec<Vec<OBJFaceVertex>> faces;
    Vec<OBJMaterial> material_data;
};

Vec<f32> wf_model_zip_v_vn_tex(OBJModel *model);

Vec<f32> wf_model_zip_v_vn(OBJModel *model);

OBJModel wf_load_obj_model(const char *path);

Vec<OBJMaterial> wf_load_obj_material_data(const char *path);

Vec<u32> wf_model_extract_indices(OBJModel *model);

Vec<f32> wf_model_extract_vertices(OBJModel *model);

Vec<f32> wf_model_extract_texcoords(OBJModel *model);

Vec<f32> wf_model_extract_normals(OBJModel *model);

#endif // WAVEFRONT_H
