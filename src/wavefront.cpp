#include <cglm/cglm.h>

#include "wavefront.h"
#include "core/string.h"

#include "io.h"

const u32 FACE_QUAD_ORDER[15] = {
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 5,
    0, 5, 6
};

u32 __compute_quad_max_end(u32 nfaces) {
    if (nfaces == 4)      return 6;
    else if (nfaces == 5) return 9;
    else if (nfaces == 6) return 12;
    else if (nfaces == 7) return 15;
    return 3;
}

u32 __parse_f32_values(Vec<f32> *dst, const char *from, const u32 limit) {
    String line = String::from(from);
    auto values = line.split(' ');
    // strip here: string_strip_free(string_from(from)); 

    u32 added = 0;
    for (u32 i=0; i<values.len() && added < limit; i++) {
        auto token = values[i];
        const f32 value = atof(token.raw());
        dst->push_back(value);
        added++;
    }

    // for (u32 i=0; i<values.len(); i++) {
    //     auto token = values[i];
    //     string_free(&token);
    // }

    return values.len();
}

void __parse_face_data(Vec<Vec<OBJFaceVertex>> *dst, const char *from) {
    Vec<OBJFaceVertex> result(4);

    String line = String::from(from);
    Vec<String> defs = line.split(' ');

    for (u32 i=0; i<defs.len(); i++) {
        OBJFaceVertex face_vertex;
        Vec<String> values = defs[i].split('/');

        if (values.len() == 2) {
            String vertex_str = values[0];
            face_vertex.vertex_id = atoi(vertex_str.raw());
            String tex_str = values[1];
            face_vertex.tex_coord_id = atoi(tex_str.raw());
        } else if (values.len() == 3) {
            String vertex_str = values[0];
            face_vertex.vertex_id = atoi(vertex_str.raw());
            String tex_str = values[1];
            face_vertex.tex_coord_id = atoi(tex_str.raw());
            String normal_str = values[2];
            face_vertex.normal_id = atoi(normal_str.raw());
        }

        // for (u32 i=0; i<values.len(); i++) {
        //     auto some_str = values[i];
        //     string_free(&some_str);
        // }
        result.push_back(face_vertex);
    }

    dst->push_back(result);
}

Vec<f32> wf_model_zip_v_vn_tex(OBJModel *model) {
    Vec<f32> result(model->faces.len() * 3);

    for (u32 i=0; i<model->faces.len(); i++) {
        Vec<OBJFaceVertex> faces = model->faces[i];

        u32 quad_max = __compute_quad_max_end(faces.len());
        for (u32 v_id=0; v_id<quad_max; v_id++) {
            const u32 v = FACE_QUAD_ORDER[v_id];
            const u32 vertex_id = (faces[v].vertex_id - 1) * 3;
            const u32 normal_id = (faces[v].normal_id - 1) * 3;
            const u32 tex_coord_id =  (faces[v].tex_coord_id - 1) * 2;

            for (u32 component_id=0; component_id<3; component_id++) {
                result.push_back(model->vertices[vertex_id+component_id]);
            }

            for (u32 component_id=0; component_id<3; component_id++) {
                if (model->normals.len() == 0)
                    result.push_back((f32)1.0f);
                else
                    result.push_back(model->normals[normal_id+component_id]);
            }
            for (u32 component_id=0; component_id<2; component_id++) {
                if (model->tex_coords.len() == 0)
                    result.push_back((f32)0.0f);
                else
                    result.push_back(model->tex_coords[tex_coord_id+component_id]);
            }
        }
    }

    return result;
}

Vec<f32> wf_model_zip_v_vn(OBJModel *model) {
    Vec<f32> result(model->faces.len() * 3);

    for (u32 i=0; i<model->faces.len(); i++) {
        Vec<OBJFaceVertex> faces = model->faces[i];
        u32 quad_max = __compute_quad_max_end(faces.len());

        for (u32 v_id=0; v_id<quad_max; v_id++) {
            const u32 v = FACE_QUAD_ORDER[v_id];
            const u32 vertex_id = (faces[v].vertex_id - 1) * 3;
            // const u32 normal_id = (faces[v].normal_id - 1) * 3;

            for (u32 component_id=0; component_id<3; component_id++)
                result.push_back(model->vertices[vertex_id+component_id]);
            for (u32 component_id=0; component_id<3; component_id++)
                result.push_back(0.0f);
        }
    }

    return result;
}

Vec<u32> wf_model_extract_indices(OBJModel *model) {
    Vec<u32> result(model->vertices.len()/3);

    for (u32 i=0; i<model->faces.len(); i++) {
        Vec<OBJFaceVertex> faces = model->faces[i];
        u32 quad_max = __compute_quad_max_end(faces.len());

        for (u32 v_id=0; v_id<quad_max; v_id++) {
            const u32 v = FACE_QUAD_ORDER[v_id];
            const u32 vertex_id = (faces[v].vertex_id - 1) * 3;
            result.push_back(vertex_id);
        }
    }

    return result;
}

Vec<f32> wf_model_extract_normals(OBJModel *model) {
    Vec<f32> result(model->faces.len() * 3);

    for (u32 i=0; i<model->faces.len(); i++) {
        Vec<OBJFaceVertex> faces = model->faces[i];

        u32 quad_max = __compute_quad_max_end(faces.len());
        for (u32 v_id=0; v_id<quad_max; v_id++) {
            const u32 v = FACE_QUAD_ORDER[v_id];
            const u32 normal_id = (faces[v].normal_id - 1) * 3;
            for (u32 component_id=0; component_id<3; component_id++) {
                if (model->normals.len() == 0)
                    result.push_back((f32)1.0f);
                else
                    result.push_back(model->normals[normal_id+component_id]);
            }
        }
    }

    return result;
}

Vec<f32> wf_model_extract_texcoords(OBJModel *model) {
    Vec<f32> result(model->faces.len() * 3);

    for (u32 i=0; i<model->faces.len(); i++) {
        Vec<OBJFaceVertex> faces = model->faces[i];
        u32 quad_max = __compute_quad_max_end(faces.len());
        for (u32 v_id=0; v_id<quad_max; v_id++) {
            const u32 v = FACE_QUAD_ORDER[v_id];
            const u32 tex_coord_id =  (faces[v].tex_coord_id - 1) * 2;
            for (u32 component_id=0; component_id<2; component_id++) {
                if (model->tex_coords.len() == 0)
                    result.push_back((f32)0.0f);
                else
                    result.push_back(model->tex_coords[tex_coord_id+component_id]);
            }
        }
    }

    return result;
}

Vec<f32> wf_model_extract_vertices(OBJModel *model) {
    Vec<f32> result(model->faces.len() * 3);

    for (u32 i=0; i<model->faces.len(); i++) {
        Vec<OBJFaceVertex> faces = model->faces[i];
        u32 quad_max = __compute_quad_max_end(faces.len());
        for (u32 v_id=0; v_id<quad_max; v_id++) {
            const u32 v = FACE_QUAD_ORDER[v_id];
            const u32 vertex_id = (faces[v].vertex_id - 1) * 3;
            for (u32 component_id=0; component_id<3; component_id++) {
                result.push_back(model->vertices[vertex_id+component_id]);
            }
        }
    }

    return result;
}

OBJModel wf_load_obj_model(const char *path) {
    OBJModel result;
    result.vertices = Vec<f32>(32);
    result.normals = Vec<f32>(32);
    result.tex_coords= Vec<f32>(32);
    result.indices = Vec<u32>(32);
    result.faces = Vec<Vec<OBJFaceVertex>>(32);

    char *content = NULL;
    u32 len = io_read_file(path, (u8 **)&content);

    if (content == NULL) {
        fprintf(stderr, "[%s:%s:%d]: Error reading file: %s\n",
                __FILE__, __FUNCTION__, __LINE__, path);
        exit(1);
    }

    for (u32 i=0; i<len; i++) {
        u32 line_len = 0;
        for (; content[i+line_len]!='\n'; line_len++);

        Vec<char> line('\0', line_len+1);
        for (; content[i] != '\n'; i++)
            line.push_back(content[i]);

        switch (line[0]) {
        case 'v': {
            char * raw_data_ptr = line._c_data.raw();
            switch(line[1]) {
            case ' ': __parse_f32_values(&result.vertices, raw_data_ptr + 2, 3); break;
            case 't': __parse_f32_values(&result.tex_coords, raw_data_ptr + 3, 2); break;
            case 'n': __parse_f32_values(&result.normals, raw_data_ptr + 3, 3); break;
            }
            break;
        }
        case 'f': {
            char * raw_data_ptr = line._c_data.raw();
            __parse_face_data(&result.faces, raw_data_ptr + 2); break;
        }
        }
    }

    return result;
}

