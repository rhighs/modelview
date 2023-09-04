#include "build/SDL/include/SDL2/SDL_mouse.h"
#include "cglm/vec3.h"
#include "io.h"
#include "scene.h"
#include <SDL2/SDL_mouse.h>
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

void checkSDLError(int line)
{
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
 
int main(int argc, char *argv[]) {
    SDL_Window *mainwindow;
    SDL_GLContext maincontext;

    const vec3 CLEAR_COLOR = { .2f, 0.2f, .2f };

    u32 win_height = 600;
    u32 win_width = 800;

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
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    // u32 indices[] = {
    //     0, 1, 3, 1, 2, 3
    // };
    // const char *image_path = "./res/mlg.png";
    // i32 image_width, image_height, nr_channels;
    // stbi_set_flip_vertically_on_load(1);
    // u8 *image_data = stbi_load(image_path, &image_width, &image_height, &nr_channels, 0);
    // if (stbi_failure_reason()) {
    //     fprintf(stderr, "Failed reading %s reason: %s\n",
    //             image_path, stbi_failure_reason());
    // }
    //
    // u32 VAO;
    // glGenVertexArrays(1, &VAO);
    //
    // glBindVertexArray(VAO);
    // u32 VBO;
    // u32 EBO; 
    // glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray(0);

    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    // u32 texture;
    // glGenTextures(1, &texture);
    // glBindTexture(GL_TEXTURE_2D, texture);
    // glTexImage2D(
    //     GL_TEXTURE_2D, 0, GL_RGB,
    //     image_width, image_height,
    //     0,
    //     GL_RGB, GL_UNSIGNED_BYTE, image_data);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //
    // stbi_image_free(image_data);
    //
    // glActiveTexture(GL_TEXTURE0);

    SDL_Event event;
    i32 running = 1;

    u64 now_time = SDL_GetPerformanceCounter();
    u64 last_time = 0;
    f64 dt_secs = 0.0;

    glEnable(GL_DEPTH_TEST);

    vec3 camera_pos = { 0.0f, 0.0f, 3.0f };

    Camera camera;

    // VAO data for light source object
    u32 light_VAO;
    u32 VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &light_VAO);

    glBindVertexArray(light_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);

    // Light coloring and shader stuff
    ShaderProgram light_program = sp_create("./vert.glsl", "./material_frag.glsl");
    sp_bind_vao(&light_program, light_VAO);

    // ShaderProgram light_source_program = sp_create("./vert.glsl", "./light_source_frag.glsl");
    // sp_bind_vao(&light_source_program, light_VAO);

    Material material = mat_make(mat_white_rubber, (vec3) { .7f, 0.0f, 0.0f });

    RenderMe rme;
    rme.transform = (Transform *)malloc(sizeof(Transform));
    rme.mesh = (Mesh *)malloc(sizeof(Mesh));
    rme.mesh->indices = NULL;
    rme.mesh->vertices = vertices;
    rme.mesh->vertex_count = 36;
    rme.vao = light_VAO;
    rme.material = &material;

    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, rme.transform->rotation);
    glm_vec3_copy((vec3) { 1.0f, 0.0f, 0.0f }, rme.transform->translation);
    glm_vec3_copy((vec3) { 1.3f, 1.0f, 1.0f }, rme.transform->scale);

    RenderMe rme_1;
    rme_1.transform = (Transform *)malloc(sizeof(Transform));
    rme_1.mesh = (Mesh *)malloc(sizeof(Mesh));
    rme_1.mesh->indices = NULL;
    rme_1.mesh->vertices = vertices;
    rme_1.mesh->vertex_count = 36;
    rme_1.vao = light_VAO;
    rme_1.material = &material;

    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, rme_1.transform->rotation);
    glm_vec3_copy((vec3) { 5.0f, 0.0f, 0.0f }, rme_1.transform->translation);
    glm_vec3_copy((vec3) { 1.3f, 1.0f, 1.0f }, rme_1.transform->scale);

    Renderer renderer;
    renderer.camera = &camera;
    renderer.program = &light_program;
    renderer.vp_width = win_width;
    renderer.vp_height = win_height;
    camera_init(renderer.camera, camera_pos);

    f32 light_x = cos(SDL_GetTicks()/1000.0f) * 1.0f;
    f32 light_z = sin(SDL_GetTicks()/1000.0f) * 1.0f;

    vec3 light_position = { light_x, 1.0f, light_z };
    vec3 light_color = { 1.0f, 1.0f, 1.0f };

    PointLight main_light = pt_light_make(light_position, light_color, light_color, light_color);
    DirectionalLight dir_light = dir_light_make((vec3) { -0.5, -0.5, -0.5 }, light_color, light_color, light_color);

    // Reading model data
    {
        char *model_data = NULL;
        const u32 content_len = io_read_file("./chicken.obj", &model_data);
        if (model_data == NULL) {
            fprintf(stderr, "Error reading file data\n");
            return -1;
        }
    }

    Scene main_scene;
    main_scene.dir_light = NULL;
    main_scene.pt_lights = (PointLight **)malloc(sizeof(PointLight *) * 10);
    main_scene.pt_lights[0] = &main_light;

    while (running) {
        last_time = now_time;
        now_time = SDL_GetPerformanceCounter();

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
                }
                case SDL_WINDOWEVENT_CLOSE: {
                    goto quit;
                }
                }
                break;
            }
            case SDL_KEYDOWN: 
                io_change_state(event.key.keysym.scancode, TRUE);
                break;
            case SDL_KEYUP: 
                io_change_state(event.key.keysym.scancode, FALSE);
                break;
            case SDL_MOUSEMOTION:
                camera_update_direction(renderer.camera, event.motion.xrel, event.motion.yrel);
                break;
            }
        }

        dt_secs = (f64)((now_time - last_time) / (f64)SDL_GetPerformanceFrequency());
        camera_update(renderer.camera, dt_secs);

        rme_1.transform->rotation[1] += 10.0 * dt_secs;

        // Test: oscillating light position
        main_scene.pt_lights[0]->position[0]
            = (1 + sin(((f64)SDL_GetTicks64())/1000.0)) * 2.0;

        glClearColor(
                CLEAR_COLOR[0],
                CLEAR_COLOR[1],
                CLEAR_COLOR[2],
                1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        rdr_draw(&renderer, &main_scene, &rme);
        rdr_draw(&renderer, &main_scene, &rme_1);

        SDL_GL_SwapWindow(mainwindow);
    }

quit:
    /* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();

    return 0;
}
