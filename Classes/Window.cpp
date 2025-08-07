#include "Window.h"
#include <iostream>

Window::Window(const char* title, int x, int y, int w, int h, bool fullscreenFlag)
    : width(w), height(h), fullscreen(fullscreenFlag) 
{
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    sdlWindow = SDL_CreateWindow(title, x, y, w, h, flags);
    if (!sdlWindow) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
    }

    setVSync(true); // default on
}

Window::~Window() {
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }
}

void Window::setFullscreen(bool enable) {
    fullscreen = enable;
    if (enable) {
        SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(sdlWindow, 0);
    }
}

void Window::setBorderless(bool borderless) {
    SDL_SetWindowBordered(sdlWindow, borderless ? SDL_FALSE : SDL_TRUE);
}

void Window::setVSync(bool enable) {
    SDL_GL_SetSwapInterval(enable ? 1 : 0);
}

void Window::setTitle(const std::string& title) {
    SDL_SetWindowTitle(sdlWindow, title.c_str());
}

void Window::move(int x, int y) {
    SDL_SetWindowPosition(sdlWindow, x, y);
}

void Window::resize(int w, int h) {
    SDL_SetWindowSize(sdlWindow, w, h);
    width = w;
    height = h;
}

int Window::getX() const {
    int x;
    SDL_GetWindowPosition(sdlWindow, &x, nullptr);
    return x;
}

int Window::getY() const {
    int y;
    SDL_GetWindowPosition(sdlWindow, nullptr, &y);
    return y;
}

int Window::getWidth() const {
    int w;
    SDL_GetWindowSize(sdlWindow, &w, nullptr);
    return w;
}

int Window::getHeight() const {
    int h;
    SDL_GetWindowSize(sdlWindow, nullptr, &h);
    return h;
}
