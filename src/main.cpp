#include "core/string.h"
#include "io.h"
#include "scene.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

/* If using gl3.h */
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#include <glad/glad.h>

#include "SDL.h"
#define PROGRAM_NAME "ModelView"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>

#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "core/types.h"
#include "camera.h"

#include "material.h"
#include "shader.h"
#include "renderer.h"

#include "mesh_utils.h"
#include "renderme.h"

#include "load.h"

#define GL_CONTEXT_MAJOR 3
#define GL_CONTEXT_MINOR 2

/* A simple function that prints a message, the error code returned by SDL,
 * and quits the application */
static void sdldie(const char *msg) {
    printf("%s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
}

static void checkSDLError(int line) {
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0') {
		printf("SDL Error: %s\n", error);
		if (line != -1)
			printf(" + line: %i\n", line);
		SDL_ClearError();
	}
#else
    (void)line;
#endif
}

#include "core/vec.h"
#include "wavefront.h"

static u32 bind_texture_info(LoadedImage image) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return texture;
}

static void update_window_size(Renderer *renderer, u32 *win_width, u32 *win_height, const SDL_WindowEvent *window_event) {
    const u32 width = window_event->data1;
    const u32 height = window_event->data2;
    *win_width = width;
    *win_height = height;
    renderer->vp_width = width;
    renderer->vp_height = height;
    glViewport(0, 0, width, height);
}

static bool process_input_events(SDL_Event *event, Renderer *renderer, u32 *win_width, u32 *win_height) {
    while (SDL_PollEvent(event) > 0) {
        ImGui_ImplSDL2_ProcessEvent(event);

        switch (event->type) {
            case SDL_WINDOWEVENT:
                switch (event->window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        update_window_size(renderer, win_width, win_height, &event->window);
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        return false;
                    default:
                        break;
                }
                break;
            case SDL_KEYDOWN:
                if (!ImGui::GetIO().WantCaptureKeyboard)
                    io_change_state(event->key.keysym.scancode, TRUE);
                break;
            case SDL_KEYUP:
                if (!ImGui::GetIO().WantCaptureKeyboard)
                    io_change_state(event->key.keysym.scancode, FALSE);
                break;
            case SDL_MOUSEMOTION:
                if (!ImGui::GetIO().WantCaptureMouse)
                    camera_update_direction(renderer->camera, event->motion.xrel, event->motion.yrel);
                break;
            default:
                break;
        }
    }

    return true;
}

struct DemoSceneState {
    Camera camera;
    Renderer renderer;
    Scene scene;
    RenderMe model;
    Vec<RenderMe *> render_list;
};

static void init_demo_scene(DemoSceneState *state, u32 win_width, u32 win_height) {
    glm::vec3 camera_pos = { 0.0f, 0.0f, 3.0f };

    String obj_model_filepath = String("./res/models/lambo/lambo.obj");
    String obj_path = Loader::resolve_filepath(obj_model_filepath.raw());
    OBJModel model_data = wf_load_obj_model(obj_path.raw());
    IO_LOG(stdout, "[MODEL_INFO]: verts = %d, normals = %d, tex_coords = %d, faces = %d",
            model_data.vertices.len(),
            model_data.normals.len(),
            model_data.tex_coords.len(),
            model_data.faces.len());

    Vec<f32> debug_points = model_data.vertices;

    Material material = mat_make(MAT_CHROME,
            glm::vec3(1.0f, .7f, 0.0f));

    state->model = rdrme_from_obj(&model_data, material, TRUE, TRUE);
    rdrme_setup_debug(&state->model, debug_points);
    state->model.transform.scale = glm::vec3(.05f, .05f, .05f);

    state->renderer = rdr_init(&state->camera, win_width, win_height);
    camera_init(state->renderer.camera, camera_pos);

    const f32 light_x = std::cos(SDL_GetTicks()/1000.0f) * 1.0f;
    const f32 light_z = std::sin(SDL_GetTicks()/1000.0f) * 1.0f;

    glm::vec3 light_position = { light_x, 10.0f, light_z };
    glm::vec3 light_color = { 1.0f, 1.0f, 1.0f };

    PointLight main_light = point_light_make(light_position, light_color, light_color, light_color);
    DirectionalLight dir_light = directional_light_make(glm::vec3(-0.5, -0.5, -0.5), light_color, light_color, light_color);

    state->scene = scene_init();
    scene_add_point_light(&state->scene, main_light);
    scene_add_directional_light(&state->scene, dir_light);

    state->render_list = Vec<RenderMe *>(32);
    state->render_list.push_back(&state->model);
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    SDL_Window *mainwindow;
    SDL_GLContext maincontext;

    const glm::vec4 CLEAR_COLOR = { .8f, 0.9f, .8f, 1.0f };

    u32 win_height = 720;
    u32 win_width = 1280;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
        sdldie("Unable to initialize SDL");

    // Request opengl 3.2 context.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_CONTEXT_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_CONTEXT_MINOR);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

    if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) == 0) {
        fprintf(stderr, "Failed to initialize glad\n");
        return -1;
    }
    gladLoadGL();

    // This makes our buffer swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);

    String png_path = Loader::resolve_filepath("./res/models/lambo/lambo.png");
    LoadedImage texture_data = io_read_image_file(png_path.raw());
    bind_texture_info(texture_data);
    stbi_image_free(texture_data.data);
    
    glActiveTexture(GL_TEXTURE0);

    SDL_Event event;
    i32 running = 1;

    u64 now_time = SDL_GetPerformanceCounter();
    u64 last_time = 0;
    f64 dt_secs = 0.0;

    glEnable(GL_DEPTH_TEST);

    DemoSceneState scene_state;
    init_demo_scene(&scene_state, win_width, win_height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(mainwindow, maincontext);
    ImGui_ImplOpenGL3_Init("#version 150");

    bool wireframe_enabled = false;
    bool show_debug_normals = scene_state.renderer.show_debug_normals == TRUE;

    f64 debug_last_toggle = 0.0;

    while (running) {
        last_time = now_time;
        now_time = SDL_GetPerformanceCounter();

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        if (!process_input_events(&event, &scene_state.renderer, &win_width, &win_height)) {
            running = 0;
            break;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(mainwindow);
        ImGui::NewFrame();

        ImGui::Begin("Debug");
        ImGui::Checkbox("Show debug normals", &show_debug_normals);
        ImGui::Checkbox("Wireframe", &wireframe_enabled);
        ImGui::SliderFloat("Camera speed", &scene_state.renderer.camera->speed, 0.1f, 20.0f);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();

        scene_state.renderer.show_debug_normals = show_debug_normals ? TRUE : FALSE;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe_enabled ? GL_LINE : GL_FILL);

        if (io_is_key_pressed(SDL_SCANCODE_F4) && debug_last_toggle > .1) {
            for (u32 rdr_i=0; rdr_i<scene_state.render_list.len(); rdr_i++)
                scene_state.render_list[rdr_i]->show_debug = !scene_state.render_list[rdr_i]->show_debug;
            debug_last_toggle = 0.0;
        } else {
            debug_last_toggle += dt_secs;
        }

        dt_secs = (f64)((now_time - last_time) / (f64)SDL_GetPerformanceFrequency());
        camera_update(scene_state.renderer.camera, dt_secs);

        const f32 y_rotation = 10.0 * dt_secs;
        scene_state.model.transform.rotation[1] += y_rotation;

        // Test: oscillating light position
        scene_state.scene.point_lights[0].position[0] = (f32)((1 + std::sin(((f64)SDL_GetTicks64())/1000.0)) * 10.0) - 5.0;
        scene_state.scene.point_lights[0].intensity = 5.0f;

        rdr_clear_color(&CLEAR_COLOR);

        for (u32 rdr_i=0; rdr_i<scene_state.render_list.len(); rdr_i++) {
            RenderMe *renderme = scene_state.render_list[rdr_i];
            rdr_draw(&scene_state.renderer, &scene_state.scene, renderme);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(mainwindow);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    /* Delete our opengl context, destroy our window, and shutdown SDL */
    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow(mainwindow);
    SDL_Quit();

    return 0;
}
