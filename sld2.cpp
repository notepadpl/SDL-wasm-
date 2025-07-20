#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>
#include <iostream>
#include <vector>

#include <stdlib.h>

#include <stdio.h>

#include <string.h>

#include <math.h>

#include <sys/mman.h>

#include <fcntl.h>

#include <unistd.h>



#include "SDL2/SDL.h"

#include "SDL_test_common.h"

#if defined(__IPHONEOS__) || defined(__ANDROID__)

#define HAVE_OPENGLES

#endif

#include "SDL_opengles.h"



struct mouse_handle {

    int x = 1;

    int y = 1;

} mouse;



int gl;



struct Vertex {

    float x, y, z;

};



static GLubyte color[8][4] = {{255, 0, 0, 0}};

std::vector<Vertex> vertices;



void glBegin(int gl) {

    vertices.clear();

}



void glVertex3f(float x, float y, float z) {

    vertices.push_back({x, y, z});

}



void glEnd() {

    switch (gl) {

        case GL_POINTS:

            for (const auto &vertex : vertices) {

                std::cout << "Point: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;

            }

            glEnableClientState(GL_COLOR_ARRAY);

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, color);

            glEnableClientState(GL_VERTEX_ARRAY);

            glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)vertices.data());

            glDrawArrays(GL_POINTS, 0, vertices.size());

            break;

        case GL_TRIANGLES:

            if (vertices.size() % 3 != 0) {

                std::cerr << "Warning: Number of vertices is not a multiple of 3 for GL_TRIANGLES" << std::endl;

            }

            for (size_t i = 0; i < vertices.size(); i += 3) {

                std::cout << "Triangle: (" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << ") -> ("

                          << vertices[i + 1].x << ", " << vertices[i + 1].y << ", " << vertices[i + 1].z << ") -> ("

                          << vertices[i + 2].x << ", " << vertices[i + 2].y << ", " << vertices[i + 2].z << ")" << std::endl;

            }

            glEnableClientState(GL_COLOR_ARRAY);

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, color);

            glEnableClientState(GL_VERTEX_ARRAY);

            glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)vertices.data());

            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

            break;

        case GL_LINES:

            if (vertices.size() % 2 != 0) {

                std::cerr << "Warning: Number of vertices is not a multiple of 2 for GL_LINES" << std::endl;

            }

            for (size_t i = 0; i < vertices.size(); i += 2) {

                std::cout << "Line: (" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << ") -> ("

                          << vertices[i + 1].x << ", " << vertices[i + 1].y << ", " << vertices[i + 1].z << ")" << std::endl;

            }

            glEnableClientState(GL_COLOR_ARRAY);

            glColorPointer(4, GL_UNSIGNED_BYTE, 0, color);

            glEnableClientState(GL_VERTEX_ARRAY);

            glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)vertices.data());

            glDrawArrays(GL_LINES, 0, vertices.size());

            break;

        default:

            std::cerr << "Unsupported primitive type!" << std::endl;

            break;

    }

}



GLuint car;

float carrot;



void loadObj(const char *fname) {

    int fd = open(fname, O_RDONLY);

    if (fd == -1) {

        perror("open");

        exit(EXIT_FAILURE);

    }



    off_t fileSize = lseek(fd, 0, SEEK_END);

    if (fileSize == -1) {

        perror("lseek");

        close(fd);

        exit(EXIT_FAILURE);

    }



    void *map = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);

    if (map == MAP_FAILED) {

        perror("mmap");

        close(fd);

        exit(EXIT_FAILURE);

    }



    close(fd);



    const char *data = (const char *)map;

    const char *end = data + fileSize;



    glPointSize(2.0);

    glPushMatrix();

    glRotatef(mouse.x % 360, mouse.y % 360, mouse.x % 360, mouse.y % 360);

    glPointSize(10.0f);

    glBegin(GL_TRIANGLES);



    while (data < end) {

        if (*data == 'v' && *(data + 1) == ' ') {

            data += 2; // pomiń "v "

            float x, y, z;

            if (sscanf(data, "%f %f %f", &x, &y, &z) == 3) {

                glVertex3f(x, y, z);

            }

        }



        // Przejdź do następnej linii

        while (data < end && *data != '\n') {

            ++data;

        }

        ++data; // Pomiń nową linię

    }



    glEnd();

    glPopMatrix();



    if (munmap(map, fileSize) == -1) {

        perror("munmap");

    }

}

void setPerspective(float fov, float aspect, float znear, float zfar) {

    float ymax = znear * tanf(fov * M_PI / 360.0f);

    float ymin = -ymax;

    float xmin = ymin * aspect;

    float xmax = ymax * aspect;

    glFrustumf(xmin, xmax, ymin, ymax, znear, zfar);

}



SDLTest_CommonState *state;

SDL_Event *event;

static SDL_GLContext *context;



struct obj {

    float x;

    float y;

    float z;



    int Render() {

        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        setPerspective(45.0f, 1.0f, 0.1f, 100.0f);

        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();

        glTranslatef(0.0f, 0.0f, -10.0f);

        glEnable(GL_LIGHTING);

        glEnable(GL_LIGHT0);

        GLfloat light_position[] = {0.2, 1.0, 10.0, 1.0};

        loadObj("/sdcard/tea.obj");

        return 1;

    }

};



int main(int argc, char *argv[]) {

    SDL_DisplayMode mode;

    state = SDLTest_CommonCreateState(argv, SDL_INIT_EVERYTHING);

    SDLTest_CommonInit(state);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 0);

    bool sdlmainloop = true;

    bool running = true;

    while (running) {

        SDL_Event event;

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {

                running = false;

            }

            if (event.type == SDL_MOUSEMOTION) {

                mouse.x = event.motion.x;

                mouse.y = event.motion.y;

            }

            SDL_GL_CreateContext(*state->windows);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            obj gg;

            gg.Render();

            SDL_GL_SwapWindow(*state->windows);

        }

    }

    //emscripten_set_main_loop(main_loop, -1, 1);


    return EXIT_SUCCESS;
}
