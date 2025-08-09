#include "DeviceInputManager.h"
#include "Window.h"

DeviceInputManager& DeviceInputManager::get() {
    static DeviceInputManager instance;
    return instance;
}

void DeviceInputManager::handleEvent(const SDL_Event& e) {
    switch (e.type) {
        case SDL_KEYDOWN:
            keyStates[e.key.keysym.scancode] = true;
            if (OnKeyPressed) OnKeyPressed(e.key.keysym.scancode);
            break;
        case SDL_KEYUP:
            keyStates[e.key.keysym.scancode] = false;
            if (OnKeyReleased) OnKeyReleased(e.key.keysym.scancode);
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouseButtonStates[e.button.button] = true;
            if (e.button.button == SDL_BUTTON_LEFT && OnMouse1Down) OnMouse1Down();
            if (e.button.button == SDL_BUTTON_RIGHT && OnMouse2Down) OnMouse2Down();
            break;
        case SDL_MOUSEBUTTONUP:
            mouseButtonStates[e.button.button] = false;
            if (e.button.button == SDL_BUTTON_LEFT && OnMouse1Release) OnMouse1Release();
            if (e.button.button == SDL_BUTTON_RIGHT && OnMouse2Release) OnMouse2Release();
            break;
        case SDL_MOUSEMOTION:
            mouseX = e.motion.x;
            mouseY = e.motion.y;
            break;
    }
}

bool DeviceInputManager::IsKeyPressed(SDL_Scancode code) const {
    auto it = keyStates.find(code);
    return it != keyStates.end() && it->second;
}

bool DeviceInputManager::IsPressedDown(Uint8 button) const {
    auto it = mouseButtonStates.find(button);
    return it != mouseButtonStates.end() && it->second;
}

int DeviceInputManager::MouseX() const {
    return mouseX;
}

int DeviceInputManager::MouseY() const {
    return mouseY;
}

int DeviceInputManager::MouseWindowX(Window* window) const {
    if (!window) return mouseX;
    int winX;
    SDL_GetWindowPosition(window->getSDLWindow(), &winX, nullptr);
    return mouseX - winX;
}

int DeviceInputManager::MouseWindowY(Window* window) const {
    if (!window) return mouseY;
    int winY;
    SDL_GetWindowPosition(window->getSDLWindow(), nullptr, &winY);
    return mouseY - winY;
}

bool DeviceInputManager::MousePressed() const {
    return mouseButtonStates.count(SDL_BUTTON_LEFT) && mouseButtonStates.at(SDL_BUTTON_LEFT) 
        || mouseButtonStates.count(SDL_BUTTON_RIGHT) && mouseButtonStates.at(SDL_BUTTON_RIGHT);
}
