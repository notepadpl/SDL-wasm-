#include <SDL.h>
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <iostream>

SDL_Window* window = nullptr;
SDL_GLContext glContext = nullptr;

void render() {
    glClearColor(0.1f, 0.2f, 0.8f, 1.0f); // niebieskawe tło
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return 1;
    }

    // Ustawienia WebGL (OpenGL ES 3.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Kolorowe okno SDL2 + GL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return 1;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext error: " << SDL_GetError() << "\n";
        return 1;
    }

    // Viewport – obowiązkowy dla WebGL!
    int width, height;
    SDL_GL_GetDrawableSize(window, &width, &height);
    glViewport(0, 0, width, height);

    std::cout << "Start render loop\n";
    emscripten_set_main_loop(render, 0, 1);

    SDL_Quit();
    return 0;
}
