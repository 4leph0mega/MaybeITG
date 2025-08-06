#include <SDL.h>
#include <sol/sol.hpp>
#include <iostream>

struct Window {
    SDL_Window* window = nullptr;
    int x, y, w, h;
    bool borderless = false;
    Uint8 r = 0, g = 0, b = 0;

    Window(const char* title, int xpos, int ypos, int width, int height, bool borderless_)
        : x(xpos), y(ypos), w(width), h(height), borderless(borderless_)
    {
        Uint32 flags = SDL_WINDOW_SHOWN;
        if (borderless) flags |= SDL_WINDOW_BORDERLESS;

        window = SDL_CreateWindow(title, x, y, w, h, flags);
        if (!window) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        }
    }

    ~Window() {
        if (window) SDL_DestroyWindow(window);
    }

    void move(int new_x, int new_y) {
        x = new_x; y = new_y;
        if (window) SDL_SetWindowPosition(window, x, y);
    }

    void set_borderless(bool enabled) {
        borderless = enabled;
        if (window) SDL_SetWindowBordered(window, enabled ? SDL_FALSE : SDL_TRUE);
    }

    void set_color(Uint8 red, Uint8 green, Uint8 blue) {
        r = red; g = green; b = blue;
    }

    void clear() {
        if (!window) return;
        SDL_Surface* surface = SDL_GetWindowSurface(window);
        if (!surface) return;

        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
        SDL_UpdateWindowSurface(window);
    }
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package);

    // Expose Window to Lua (Sol 3.2.3 style)
    lua.new_usertype<Window>("Window",
        "new", [](const char* title, int xpos, int ypos, int width, int height, bool borderless_) {
            return new Window(title, xpos, ypos, width, height, borderless_);
        },
        "move", &Window::move,
        "set_borderless", &Window::set_borderless,
        "set_color", &Window::set_color,
        "clear", &Window::clear
    );

    sol::load_result script = lua.load_file("Game.lua");
    if (!script.valid()) {
        sol::error err = script;
        std::cerr << "Failed to load game.lua: " << err.what() << std::endl;
        SDL_Quit();
        return 1;
    }
    script();

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }
        SDL_Delay(10);
    }

    SDL_Quit();
    return 0;
}
