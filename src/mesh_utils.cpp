#include "mesh_utils.h"

#ifdef MUDEBUG
#include "io.h"
#endif

static
glm::vec3 __mu_gen_face_normal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    glm::vec3 v1v2 = v2 - v1;
    glm::vec3 v1v3 = v3 - v1;
    glm::vec3 result = glm::cross(v1v2, v1v3);
    return result;
}

Vec<f32> mu_gen_normals(const Vec<f32>& vertices) {
    const u32 vertices_len = vertices.len();
    DEV_ASSERT(vertices_len > 8, "vertices len must be > 0, cannot generate normals for 0 verts...");
    Vec<f32> result(vertices.len());

    for (u32 i=0; i<vertices_len-8; i+=9) {
        glm::vec3 v1(vertices[i+0], vertices[i+1], vertices[i+2]);
        glm::vec3 v2(vertices[i+3], vertices[i+4], vertices[i+5]);
        glm::vec3 v3(vertices[i+6], vertices[i+7], vertices[i+8]);

        glm::vec3 normal = __mu_gen_face_normal(v1, v2, v3);
        result.push_back(normal.x); result.push_back(normal.y); result.push_back(normal.z);
        result.push_back(normal.x); result.push_back(normal.y); result.push_back(normal.z);
        result.push_back(normal.x); result.push_back(normal.y); result.push_back(normal.z);
    }

    return result;
}

void mu_interpolate_normals(Vec<f32>& normals, Vec<u32>& indices) {
    u32 max_i = 0;
    for (u32 i=0; i<indices.len(); i++)
        if (max_i<indices[i]) max_i=indices[i];
    Vec<f32> tmp(0.0f, max_i * 3);

    for (u32 i=0; i<indices.len(); i++) {
        u32 _i = indices[i];
        f32 x=normals[i*3+0], y=normals[i*3+1], z=normals[i*3+2];
        tmp[_i+0]+=x; tmp[_i+1]+=y; tmp[_i+2]+=z;
    }

    for (u32 i=0; i<tmp.len()-3; i+=3) {
        glm::vec3 vn = { tmp[i+0], tmp[i+1], tmp[i+2] };
        vn = glm::normalize(vn);
        tmp[i+0]=vn[0]; tmp[i+1]=vn[1]; tmp[i+2]=vn[2];
    }

    for (u32 i=0; i<indices.len(); i++) {
        const u32 _i = indices[i];
        normals[i*3+0] = tmp[_i+0];
        normals[i*3+1] = tmp[_i+1];
        normals[i*3+2] = tmp[_i+2];
    }

#ifdef MUDEBUG
    IO_LOG(stdout, "vn = %d, idxs = %d, result = %d\n",
            normals.len/3, indices.len(), tmp.len());
#endif
}

