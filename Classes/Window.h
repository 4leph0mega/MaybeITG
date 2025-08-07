#pragma once
#include <SDL.h>
#include <string>

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

private:
    SDL_Window* sdlWindow = nullptr;
    int width = 800;
    int height = 600;
    bool fullscreen = false;
};
