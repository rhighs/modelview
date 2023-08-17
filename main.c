#include "cglm/call/io.h"
#include "cglm/io.h"
#include "cglm/mat4.h"
#include "cglm/util.h"
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
	if (*error != '\0')
	{
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

typedef struct {
    u32 program;
    u32 VAO;
} ShaderProgram;

void sp_bind_vao(ShaderProgram *program, const u32 VAO) { program->VAO = VAO; }

void sp_use(ShaderProgram *program) {
    glUseProgram(program->program);
}

void sp_set_uniform_vec4f(ShaderProgram *program, const char* uniform,
        const float x, const float y, const float z, const float w) {
    const i32 uniform_location = glGetUniformLocation(program->program, uniform);
    glUniform4f(uniform_location, x, y, z, w);
}

void sp_set_uniform_mat4(ShaderProgram *program, const char* uniform,
        const mat4 mat) {
    const i32 uniform_location = glGetUniformLocation(program->program, uniform);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, (float *)mat);
}

ShaderProgram sp_create(
        const char *vert_source_path,
        const char *frag_source_path
        ) {
    i32 success;
    char info_log[512];

    const char *vertex_shader_content = read_shader_file(vert_source_path);
    const u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_content, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        fprintf(stderr, "[SHADER_C_ERR] %s: %s\ncontent:%s\n",
                vert_source_path, info_log, vertex_shader_content);
    }

    const char *fragment_shader_content = read_shader_file(frag_source_path);
    const u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_content, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        fprintf(stderr, "[SHADER_C_ERR] %s: %s\ncontent:%s\n",
                frag_source_path, info_log, fragment_shader_content);
    }

    const u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        fprintf(stderr, "[PROGRAM_LINK_ERR]: %s\n", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return (ShaderProgram) { .program = shader_program };
}
 
int main(int argc, char *argv[]) {
    SDL_Window *mainwindow;
    SDL_GLContext maincontext;

    const int win_height = 600;
    const int win_width = 800;

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
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

    const char *image_path = "./res/mlg.png";
    i32 image_width, image_height, nr_channels;
    stbi_set_flip_vertically_on_load(1);
    u8 *image_data = stbi_load(image_path, &image_width, &image_height, &nr_channels, 0);
    if (stbi_failure_reason()) {
        fprintf(stderr, "Failed reading %s reason: %s\n",
                image_path, stbi_failure_reason());
    }

    // This makes our buffer swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
    };

    u32 indices[] = {
        0, 1, 3, 1, 2, 3
    };

    ShaderProgram shader_program = sp_create("./vert.glsl", "./frag.glsl");

    u32 VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    u32 VBO;
    u32 EBO; 
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        image_width, image_height,
        0,
        GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    sp_bind_vao(&shader_program, VAO);
    stbi_image_free(image_data);

    glActiveTexture(GL_TEXTURE0);

    SDL_Event event;
    i32 running = 1;

    u64 now_time = SDL_GetPerformanceCounter();
    u64 last_time = 0;
    f64 dt_secs = 0.0;

    mat4 transform = {};
    glm_mat4_identity(transform);
    // vec3 translation_vec = { 0.4f, 0.2f, 0.0f };
    // glm_translate(transform, translation_vec);

    while (running) {
        last_time = now_time;
        now_time = SDL_GetPerformanceCounter();

        while (SDL_PollEvent(&event) > 0) {
            if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED : {
                    const u32 w = event.window.data1;
                    const u32 h = event.window.data2;
                    glViewport(0, 0, w, h);
                }
                case SDL_WINDOWEVENT_CLOSE: {
                    goto quit;
                }
                }
            }
        }

        dt_secs = (f64)((now_time - last_time) / (f64)SDL_GetPerformanceFrequency());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4 stub = {};
        vec3 axis = { 0.0f, 0.0f, 1.0f };
        glm_mat4_copy(transform, stub);
        const f64 sinme = (f64)SDL_GetTicks()/1000.0 + 0.5f;
        f32 scale_factor = sinf(sinme);
        vec3 scale = { scale_factor, scale_factor, scale_factor };
        glm_scale(stub, scale);
        glm_rotate(stub, 10.0f * dt_secs , axis);
        glm_rotate(transform, 10.0f * dt_secs , axis);

        glBindTexture(GL_TEXTURE_2D, texture);
        sp_use(&shader_program);
        sp_set_uniform_mat4(&shader_program, "transform", stub);
        {
            glBindVertexArray(shader_program.VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
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
