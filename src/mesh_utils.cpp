#include "mesh_utils.h"

#ifdef MUDEBUG
#include "io.h"
#endif

static
void __mu_gen_face_normal(vec3 v1, vec3 v2, vec3 v3, vec3 result) {
    vec3 v1v2;
    vec3 v1v3;
    glm_vec3_sub(v2, v1, v1v2);
    glm_vec3_sub(v3, v1, v1v3);
    glm_vec3_cross(v1v2, v1v3, result);
    glm_vec3_norm(result);
}

Vec<f32> mu_gen_normals(Vec<f32> vertices) {
    Vec<f32> result;
    array_init(&result, vertices.len);

    for (u32 i=0; i<vertices.len-8; i+=9) {
        vec3 v1 = { vertices[i+0], vertices[i+1], vertices[i+2] };
        vec3 v2 = { vertices[i+3], vertices[i+4], vertices[i+5] };
        vec3 v3 = { vertices[i+6], vertices[i+7], vertices[i+8] };

        vec3 normal;
        __mu_gen_face_normal(v1, v2, v3, normal);
        array_push(&result, normal[0]); array_push(&result, normal[1]); array_push(&result, normal[2]);
        array_push(&result, normal[0]); array_push(&result, normal[1]); array_push(&result, normal[2]);
        array_push(&result, normal[0]); array_push(&result, normal[1]); array_push(&result, normal[2]);
    }

    return result;
}

void mu_interpolate_normals(Vec<f32> normals, Vec<u32> indices) {
    Vec<f32> tmp;

    u32 max_i = 0;
    for (u32 i=0; i<indices.len; i++)
        if (max_i<indices[i]) max_i=indices[i];
    array_init_with(&tmp, 0.0f, max_i * 3);
    array_fill_with(&tmp, 0.0f, max_i * 3);

    for (u32 i=0; i<indices.len; i++) {
        u32 _i = indices[i];
        f32 x=normals[i*3+0], y=normals[i*3+1], z=normals[i*3+2];
        tmp[_i+0]+=x; tmp[_i+1]+=y; tmp[_i+2]+=z;
    }

    for (u32 i=0; i<tmp.len-3; i+=3) {
        vec3 vn = { tmp[i+0], tmp[i+1], tmp[i+2] };
        glm_vec3_normalize(vn);
        tmp[i+0]=vn[0]; tmp[i+1]=vn[1]; tmp[i+2]=vn[2];
    }

    for (u32 i=0; i<indices.len; i++) {
        const u32 _i = indices[i];
        normals[i*3+0] = tmp[_i+0];
        normals[i*3+1] = tmp[_i+1];
        normals[i*3+2] = tmp[_i+2];
    }

#ifdef MUDEBUG
    IO_LOG(stdout, "vn = %d, idxs = %d, result = %d\n",
            normals.len/3, indices.len, tmp.len);
#endif

    array_free(&tmp);
}

