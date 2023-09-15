#include "io.h"
#include "scene.h"
#include <SDL2/SDL_mouse.h>
#include <cctype>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

/* If using gl3.h */
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#include <glad/glad.h>

#include <SDL2/SDL.h>
#define PROGRAM_NAME "test"

#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "types.h"
#include "camera.h"

#include "material.h"
#include "shader.h"
#include "renderer.h"

#include "string.h"

#include "renderme.h"

Material mat_white_plastic = {
    {0.0, 0.0, 0.0},
    {0.55, 0.55, 0.55},
    {0.70, 0.70, 0.70},
    0.25
};

Material mat_chrome = {
    {0.25, 0.25, 0.25},
    {0.4, 0.4, 0.4},
    {0.774597, 0.774597, 0.774597},
    0.6
};

Material mat_white_rubber = {
    {0.05, 0.05, 0.05},
    {0.5, 0.5, 0.5},
    {0.7, 0.7, 0.7},
    0.078125
};

typedef enum {
    PLASTIC,
    METAL,
    RUBBER,
} MaterialType;

Material mat_make(Material base_material, vec3 color){
    Material mat = {};

    glm_vec3_mul(base_material.ambient, color, mat.ambient);
    glm_vec3_mul(base_material.diffuse, color, mat.diffuse);
    glm_vec3_mul(base_material.specular, color, mat.specular);
    mat.shininess = base_material.shininess;

    return mat;
}

/* A simple function that prints a message, the error code returned by SDL,
 * and quits the application */
void sdldie(const char *msg) {
    printf("%s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
}

void checkSDLError(int line) {
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0') {
		printf("SDL Error: %s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
	}
#endif
}

#include "array.h"

struct FaceVertex {
    u32 vertex_id;
    u32 tex_coord_id;
    u32 normal_id;
};

struct Model {
    Array<f32> vertices;
    Array<f32> normals;
    Array<f32> tex_coords;
    Array<u32> indices;
    Array<Array<FaceVertex>> faces;
};

u32 wf_parse_f32_values(Array<f32> *dst, const char *from, const u32 limit) {
    String line = string_strip_free(string_from(from));
    auto values = string_split(line, ' ');

    u32 added = 0;
    for (u32 i=0; i<values.len && added < limit; i++) {
        auto token = values[i];
        const f32 value = atof(string_c(&token));
        array_push(dst, value);
        added++;
    }

    for (u32 i=0; i<values.len; i++) {
        auto token = values[i];
        string_free(&token);
    }
    array_free(&values);

    return values.len;
}

void wf_parse_face_data(Array<Array<FaceVertex>> *dst, const char *from) {
    Array<FaceVertex> result;
    array_init(&result, 4);

    String line = string_from(from);
    Array<String> defs = string_split(line, ' ');

    for (u32 i=0; i<defs.len; i++) {
        FaceVertex face_vertex;
        Array<String> values = string_split(defs[i], '/');

        if (values.len == 2) {
            String vertex_str = values[0];
            face_vertex.vertex_id = atoi(string_c(&vertex_str));
            String tex_str = values[1];
            face_vertex.tex_coord_id = atoi(string_c(&tex_str));
        } else if (values.len == 3) {
            String vertex_str = values[0];
            face_vertex.vertex_id = atoi(string_c(&vertex_str));
            String tex_str = values[1];
            face_vertex.tex_coord_id = atoi(string_c(&tex_str));
            String normal_str = values[2];
            face_vertex.normal_id = atoi(string_c(&normal_str));
        }

        for (u32 i=0; i<values.len; i++) {
            auto some_str = values[i];
            string_free(&some_str);
        }
        array_free(&values);
        array_push(&result, face_vertex);
    }

    for (u32 i=0; i<defs.len; i++) {
        auto some_str = defs[i];
        string_free(&some_str);
    }
    array_free(&defs);
    array_push(dst, result);
}

void load_wf_obj_model(const char *path, Model *dst) {
    array_init(&(dst->vertices),    32);
    array_init(&(dst->normals),     32);
    array_init(&(dst->tex_coords),  32);
    array_init(&(dst->indices),     32);
    array_init(&(dst->faces),       32);

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

        Array<char> line;
        array_init_with(&line, '\0', line_len+1);
        for (; content[i] != '\n'; i++)
            array_push(&line, content[i]);

        switch (line.data[0]) {
        case 'v':
            switch(line.data[1]) {
            case ' ': wf_parse_f32_values(&dst->vertices, line.data + 2, 3); break;
            case 't': wf_parse_f32_values(&dst->tex_coords, line.data + 3, 2); break;
            case 'n': wf_parse_f32_values(&dst->normals, line.data + 3, 3); break;
            }
            break;
        case 'f':
            wf_parse_face_data(&dst->faces, line.data + 2); break;
        }
    }
}

Array<f32> model_zip_v_vn_tex(Model *model) {
    Array<f32> result;
    array_init(&result, model->faces.len * 3);
    const u32 quad[15] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6
    };

#if 0
    if (model->normals.len == 0) {
        return {};
    }
#endif

    for (u32 i=0; i<model->faces.len; i++) {
        Array<FaceVertex> faces = model->faces[i];

        u32 quad_max = 3;
        if (faces.len == 4)      quad_max = 6;
        else if (faces.len == 5) quad_max = 9;
        else if (faces.len == 6) quad_max = 12;
        else if (faces.len == 7) quad_max = 15;

        for (u32 v_id=0; v_id<quad_max; v_id++) {
            const u32 v = quad[v_id];
            const u32 vertex_id = (faces[v].vertex_id - 1) * 3;
            const u32 normal_id = (faces[v].normal_id - 1) * 3;
            const u32 tex_coord_id =  (faces[v].tex_coord_id - 1) * 2;

            for (u32 component_id=0; component_id<3; component_id++) {
                array_push(&result, model->vertices[vertex_id+component_id]);
            }

            for (u32 component_id=0; component_id<3; component_id++) {
                if (model->normals.len == 0)
                    array_push(&result, (f32)0.0f);
                else
                    array_push(&result, model->normals[normal_id+component_id]);
            }
            for (u32 component_id=0; component_id<2; component_id++) {
                if (model->tex_coords.len == 0)
                    array_push(&result, (f32)0.0f);
                else
                    array_push(&result, model->tex_coords[tex_coord_id+component_id]);
            }
        }
    }

    return result;
}

Array<f32> model_zip_v_vn(Model *model) {
    Array<f32> result;
    array_init(&result, model->faces.len * 3);
    const u32 quad[15] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6
    };

#if 0
    if (model->normals.len == 0) {
        return {};
    }
#endif

    for (u32 i=0; i<model->faces.len; i++) {
        Array<FaceVertex> faces = model->faces[i];

        u32 quad_max = 3;
        if (faces.len == 4)      quad_max = 6;
        else if (faces.len == 5) quad_max = 9;
        else if (faces.len == 6) quad_max = 12;
        else if (faces.len == 7) quad_max = 15;

        for (u32 v_id=0; v_id<quad_max; v_id++) {
            const u32 v = quad[v_id];
            const u32 vertex_id = (faces[v].vertex_id - 1) * 3;
            // const u32 normal_id = (faces[v].normal_id - 1) * 3;

            for (u32 component_id=0; component_id<3; component_id++)
                array_push(&result, model->vertices[vertex_id+component_id]);
            for (u32 component_id=0; component_id<3; component_id++)
                array_push(&result, 0.0f);
        }
    }

    return result;
}

char *read_shader_file(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    u32 filesize = ftell(file);
    rewind(file);

    char *shader_content = (char *)malloc(filesize+1);
    if (!fread(shader_content, sizeof(char), filesize, file)) {
        fprintf(stderr, "Could not read shader file: %s\n", filepath);
        exit(-1);
    }
    shader_content[filesize] = '\0';

    return shader_content;
}

u32 bind_texture_info(LoadedImage image) {
    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        image.width, image.height,
        0,
        GL_RGB, GL_UNSIGNED_BYTE, image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    return texture;
}
 
int main(int argc, char *argv[]) {
    SDL_Window *mainwindow;
    SDL_GLContext maincontext;

    const vec3 CLEAR_COLOR = { .8f, 0.9f, .8f };

    u32 win_height = 720;
    u32 win_width = 1280;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
        sdldie("Unable to initialize SDL");

    // Request opengl 3.2 context.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mainwindow = SDL_CreateWindow(PROGRAM_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        win_width, win_height,
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_SHOWN
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_MOUSE_CAPTURE
        | SDL_WINDOW_INPUT_GRABBED);

    SDL_SetRelativeMouseMode((SDL_bool)TRUE);

    SDL_ShowCursor(TRUE);
    if (!mainwindow) /* Die if creation failed */
        sdldie("Unable to create window");

    checkSDLError(__LINE__);

    /* Create our opengl context and attach it to our window */
    maincontext = SDL_GL_CreateContext(mainwindow);
    checkSDLError(__LINE__);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        fprintf(stderr, "Failed to initialize glad\n");
        return -1;
    }
    gladLoadGL();

    // This makes our buffer swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);

    float vertices[] = {
        // Back quad
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        // Front quad
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        // Left side quad
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,

        // Right side quad
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        // Bottom quad
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,

        // Top quad
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    };
    
    auto texture_data = io_read_image_file("./res/models/lambo/lambo.png");
    u32 _texture = bind_texture_info(texture_data);
    stbi_image_free(texture_data.data);
    
    glActiveTexture(GL_TEXTURE0);

    SDL_Event event;
    i32 running = 1;

    u64 now_time = SDL_GetPerformanceCounter();
    u64 last_time = 0;
    f64 dt_secs = 0.0;

    glEnable(GL_DEPTH_TEST);

    vec3 camera_pos = { 0.0f, 0.0f, 3.0f };

    Camera camera;

    Model mymodel;
    load_wf_obj_model("./res/models/lambo/lambo.obj", &mymodel);
    printf("[MODEL_INFO]: verts = %d, normals = %d, tex_coords = %d, faces = %d\n",
            mymodel.vertices.len,
            mymodel.normals.len,
            mymodel.tex_coords.len,
            mymodel.faces.len);
    auto result = model_zip_v_vn_tex(&mymodel);

    printf("[MODEL_INFO]: no. triangles = %d\n", (result.len/6)/3);

    // Light coloring and shader stuff
    Material material = mat_make(mat_white_rubber, (vec3) { 1.f, 1.0f, 1.0f });

    RenderMe rme = rdrme_create(result,
        RDRME_LIGHT | RDRME_TEXTURE | RDRME_NORMAL,
        material);
    glm_vec3_copy((vec3) { .05f, .05f, .05f }, rme.transform.scale);

    Array<f32> vertices_arr = array_from_copy(vertices, RAW_ARRAY_LEN(vertices));

    Material debug_material = mat_make(mat_white_rubber, (vec3) { 1.0f, 0.0f, 0.0f });
    RenderMe debug_box = rdrme_create(
        vertices_arr,
        RDRME_LIGHT | RDRME_NORMAL,
        debug_material
        );

    const f32 scale_coeff = 0.05;
    debug_box.transform.scale[0] = scale_coeff;
    debug_box.transform.scale[1] = scale_coeff;
    debug_box.transform.scale[2] = scale_coeff;

    Renderer renderer = rdr_init(&camera, win_width, win_height);
    camera_init(renderer.camera, camera_pos);

    f32 light_x = cos(SDL_GetTicks()/1000.0f) * 1.0f;
    f32 light_z = sin(SDL_GetTicks()/1000.0f) * 1.0f;

    vec3 light_position = { light_x, 1.0f, light_z };
    vec3 light_color = { 1.0f, 1.0f, 1.0f };

    PointLight main_light = point_light_make(light_position, light_color, light_color, light_color);
    DirectionalLight dir_light = directional_light_make((vec3) { -0.5, -0.5, -0.5 }, light_color, light_color, light_color);

    Scene main_scene = scene_init();
    scene_add_point_light(&main_scene, main_light);
    scene_add_directional_light(&main_scene, dir_light);

    while (running) {
        last_time = now_time;
        now_time = SDL_GetPerformanceCounter();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        while (SDL_PollEvent(&event) > 0) {
            switch (event.type) {
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED : {
                    const u32 w = event.window.data1;
                    const u32 h = event.window.data2;
                    win_width = w;
                    win_height = h;
                    renderer.vp_width= w;
                    renderer.vp_height = h;
                    glViewport(0, 0, w, h);
                    break;
                }
                case SDL_WINDOWEVENT_CLOSE: {
                    goto quit;
                }
                }
                break;
            }
            case SDL_KEYDOWN: 
                io_change_state(event.key.keysym.scancode, TRUE); break;
            case SDL_KEYUP: 
                io_change_state(event.key.keysym.scancode, FALSE); break;
            case SDL_MOUSEMOTION:
                camera_update_direction(renderer.camera, event.motion.xrel, event.motion.yrel); break;
            }
        }

        dt_secs = (f64)((now_time - last_time) / (f64)SDL_GetPerformanceFrequency());
        camera_update(renderer.camera, dt_secs);

        const f32 y_rotation = 10.0 * dt_secs;
        rme.transform.rotation[1] += y_rotation;

        // Test: oscillating light position
        main_scene.point_lights[0].position[0] = (f32)((1 + sin(((f64)SDL_GetTicks64())/1000.0)) * 2.0);

        glClearColor(
                CLEAR_COLOR[0],
                CLEAR_COLOR[1],
                CLEAR_COLOR[2],
                1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TEMP: debug draw object vertices
        for (u32 i=0; i<mymodel.vertices.len-3; i+=3) {
            glm_vec3_copy((vec3) {
                    mymodel.vertices[i+0] * rme.transform.scale[0],
                    mymodel.vertices[i+1] * rme.transform.scale[1],
                    mymodel.vertices[i+2] * rme.transform.scale[2]
                    },
                    debug_box.transform.translation);
            glm_vec3_rotate(debug_box.transform.translation, glm_rad(rme.transform.rotation[2]), (vec3){ 0.0f, 0.0f, 1.0f });
            glm_vec3_rotate(debug_box.transform.translation, glm_rad(rme.transform.rotation[1]), (vec3){ 0.0f, 1.0f, 0.0f });
            glm_vec3_rotate(debug_box.transform.translation, glm_rad(rme.transform.rotation[0]), (vec3){ 1.0f, 0.0f, 0.0f });
            glm_vec3_add(debug_box.transform.translation, rme.transform.translation, debug_box.transform.translation);
            glm_vec3_copy(rme.transform.rotation, debug_box.transform.rotation);
            rdr_draw(&renderer, &main_scene, &debug_box);
        }

        for (u32 n_lambos=0; n_lambos<4; n_lambos++) {
            glm_vec3_copy((vec3) { 40.0f * (f32)n_lambos, 0.0f, 0.0f }, rme.transform.translation);
            rdr_draw(&renderer, &main_scene, &rme);
        }

        SDL_GL_SwapWindow(mainwindow);
    }

quit:
    /* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();

    return 0;
}
