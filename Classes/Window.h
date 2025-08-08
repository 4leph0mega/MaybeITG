#pragma once
#include <SDL.h>
#include <string>
#include <glad/gl.h>  // glad header for OpenGL functions
#include "imagebuffer.h"

class Window {
public:
    Window(const char* title, int x, int y, int w, int h, bool fullscreen);
    ~Window();

    void setFullscreen(bool fullscreen);
    void setBorderless(bool borderless);
    void setVSync(bool enable);
    void setTitle(const std::string& title);
    void move(int x, int y);
    void resize(int w, int h);

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

    SDL_Window* getSDLWindow() const { return sdlWindow; }

    void makeContextCurrent();
    void swapBuffers();

    void renderImageBuffer(const ImageBuffer& imgbuf);

private:
    SDL_Window* sdlWindow = nullptr;
    SDL_GLContext glContext = nullptr;

    int width = 800;
    int height = 600;
    int posX = SDL_WINDOWPOS_CENTERED;
    int posY = SDL_WINDOWPOS_CENTERED;
    bool fullscreen = false;

    // OpenGL resources for rendering textured quad
    GLuint shaderProgram = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    void initOpenGL();
    void initRenderResources();
};
