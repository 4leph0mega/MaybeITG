#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glad/gl.h>
#include <lua.hpp>
#include <LuaBridge3/LuaBridge.h>
#include <iostream>

#include "Classes/Window.h"
#include "Classes/DeviceInputManager.h"
#include "Classes/ImageBuffer.h"

lua_State* L = nullptr;
luabridge::LuaRef* luaUpdate = nullptr;  // pointer, initially nullptr

void runMainLoop() {
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            DeviceInputManager::get().handleEvent(e);
        }

        if (luaUpdate && luaUpdate->isFunction()) {
            try {
                (*luaUpdate)();
            } catch (const std::exception& err) {
                std::cerr << "Lua Update error: " << err.what() << std::endl;
                running = false;
            }
        }

        SDL_Delay(16); // ~60 FPS
    }
}

void bindToLua(lua_State* L) {
    using namespace luabridge;

    getGlobalNamespace(L)
        .beginClass<Window>("Window")
            .addConstructor<void(*)(const char*, int, int, int, int, bool)>()
            .addFunction("setFullscreen", &Window::setFullscreen)
            .addFunction("setBorderless", &Window::setBorderless)
            .addFunction("setVSync", &Window::setVSync)
            .addFunction("setTitle", &Window::setTitle)
            .addFunction("move", &Window::move)
            .addFunction("resize", &Window::resize)
            .addFunction("getX", &Window::getX)
            .addFunction("getY", &Window::getY)
            .addFunction("getWidth", &Window::getWidth)
            .addFunction("getHeight", &Window::getHeight)
            .addFunction("renderImageBuffer", &Window::renderImageBuffer)
        .endClass()

        .beginClass<DeviceInputManager>("DeviceInput")
            .addStaticFunction("get", &DeviceInputManager::get)
            .addFunction("IsKeyPressed", &DeviceInputManager::IsKeyPressed)
            .addFunction("IsPressedDown", &DeviceInputManager::IsPressedDown)
            .addFunction("MouseX", &DeviceInputManager::MouseX)
            .addFunction("MouseY", &DeviceInputManager::MouseY)
            .addFunction("MouseWindowX", &DeviceInputManager::MouseWindowX)
            .addFunction("MouseWindowY", &DeviceInputManager::MouseWindowY)
            .addFunction("MousePressed", &DeviceInputManager::MousePressed)
        .endClass()

        .beginClass<ImageBuffer>("ImageBuffer")
            .addConstructor<void(*)()>()
            .addFunction("loadFromFile", &ImageBuffer::loadFromFile)
            .addFunction("getWidth", &ImageBuffer::getWidth)
            .addFunction("getHeight", &ImageBuffer::getHeight)
            .addFunction("getTextureID", &ImageBuffer::getTextureID)
        .endClass();
}

int main() {
    L = luaL_newstate();
    if (!L) {
        std::cerr << "Failed to create Lua state" << std::endl;
        return 1;
    }
    luaL_openlibs(L);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        lua_close(L);
        return 1;
    }

    bindToLua(L);

    if (luaL_dofile(L, "game.lua") == LUA_OK) {
        // Create the LuaRef on the heap, initialized with Lua state
        luaUpdate = new luabridge::LuaRef(luabridge::getGlobal(L, "Update"));
    } else {
        std::cerr << "Error loading game.lua: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }

    runMainLoop();

    // Clean up LuaRef pointer
    delete luaUpdate;

    lua_close(L);
    SDL_Quit();
    return 0;
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return main();
}
#endif
