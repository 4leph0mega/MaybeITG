#pragma once
#include <unordered_map>
#include <functional>
#include <SDL.h>

class Window; // forward declare

class DeviceInputManager {
public:
    static DeviceInputManager& get();

    void handleEvent(const SDL_Event& e);

    bool IsKeyPressed(SDL_Scancode code) const;
    bool IsPressedDown(Uint8 button) const;
    int MouseX() const;
    int MouseY() const;
    int MouseWindowX(Window* window) const;
    int MouseWindowY(Window* window) const;
    bool MousePressed() const;

    // Callbacks to Lua or other C++ handlers
    std::function<void()> OnMouse1Down;
    std::function<void()> OnMouse1Release;
    std::function<void()> OnMouse2Down;
    std::function<void()> OnMouse2Release;
    std::function<void(SDL_Scancode)> OnKeyPressed;
    std::function<void(SDL_Scancode)> OnKeyReleased;

private:
    DeviceInputManager() = default;

    std::unordered_map<SDL_Scancode, bool> keyStates;
    std::unordered_map<Uint8, bool> mouseButtonStates;

    int mouseX = 0;
    int mouseY = 0;
};
