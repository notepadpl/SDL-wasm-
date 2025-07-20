#include <SDL.h>
#include <SDL_opengl.h>
#include <emscripten.h>

// Screen dimensions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// SDL window and OpenGL context
SDL_Window* gWindow = nullptr;
SDL_GLContext gContext;

bool init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Use OpenGL ES 2.0
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    // Create window
    gWindow = SDL_CreateWindow("Blue OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create OpenGL context
    gContext = SDL_GL_CreateContext(gWindow);
    if (gContext == nullptr) {
        printf("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize OpenGL
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Blue color

    return true;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
    SDL_GL_SwapWindow(gWindow);   // Update the screen
}

void close() {
    // Destroy context and window
    SDL_GL_DeleteContext(gContext);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}

void main_loop() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            emscripten_cancel_main_loop(); // Stop the loop if the user quits
        }
    }
    render();
}

int main(int argc, char* args[]) {
    if (!init()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    // Use emscripten_set_main_loop for continuous rendering in the browser
    emscripten_set_main_loop(main_loop, 0, 1); // 0 for infinite loop, 1 for simulate_infinite_loop

    // The close function will be called when the browser tab is closed or the program exits
    // due to emscripten_cancel_main_loop or a manual exit.
    atexit(close);

    return 0;
}
