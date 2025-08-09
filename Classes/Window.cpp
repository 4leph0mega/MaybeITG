#include "window.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Vertex shader source (simple textured quad)
static const char* vertexShaderSrc = R"glsl(
#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
out vec2 fragUV;
void main() {
    fragUV = inUV;
    gl_Position = vec4(inPos, 0.0, 1.0);
}
)glsl";

// Fragment shader source (texture sampler)
static const char* fragmentShaderSrc = R"glsl(
#version 330 core
in vec2 fragUV;
out vec4 outColor;
uniform sampler2D uTexture;
void main() {
    outColor = texture(uTexture, fragUV);
}
)glsl";

static GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint createShaderProgram(const char* vertSrc, const char* fragSrc) {
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertSrc);
    if (!vertShader) return 0;

    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader) {
        glDeleteShader(vertShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program link error: " << infoLog << std::endl;
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

// Vertex data for fullscreen quad (pos.xy, uv.xy)
static const float quadVertices[] = {
    // positions  // uvs
    -1.f, -1.f,    0.f, 0.f,
     1.f, -1.f,    1.f, 0.f,
     1.f,  1.f,    1.f, 1.f,
    -1.f,  1.f,    0.f, 1.f
};
static const GLuint quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};

Window::Window(const char* title, int x, int y, int w, int h, bool fullscreen)
    : posX(x), posY(y), width(w), height(h), fullscreen(fullscreen)
{
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;

    sdlWindow = SDL_CreateWindow(title, posX, posY, width, height, flags);
    if (!sdlWindow) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glContext = SDL_GL_CreateContext(sdlWindow);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        return;
    }

    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize glad" << std::endl;
        return;
    }

    SDL_GL_SetSwapInterval(1); // VSync enabled

    initRenderResources();
}

Window::~Window() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (sdlWindow) SDL_DestroyWindow(sdlWindow);
}

void Window::makeContextCurrent() {
    if (sdlWindow && glContext)
        SDL_GL_MakeCurrent(sdlWindow, glContext);
}

void Window::swapBuffers() {
    if (sdlWindow)
        SDL_GL_SwapWindow(sdlWindow);
}

void Window::initRenderResources() {
    shaderProgram = createShaderProgram(vertexShaderSrc, fragmentShaderSrc);
    if (!shaderProgram) {
        std::cerr << "Failed to create shader program" << std::endl;
        return;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Window::renderImageBuffer(const ImageBuffer& imgbuf) {
    if (!shaderProgram) return;

    GLuint tex = imgbuf.getTextureID();
    if (tex == 0) {
        std::cerr << "Warning: trying to render texture 0 (invalid texture)" << std::endl;
        return;
    }

    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLint texLoc = glGetUniformLocation(shaderProgram, "uTexture");
    glUniform1i(texLoc, 0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    swapBuffers();
}

void Window::setFullscreen(bool enable) {
    if (enable != fullscreen) {
        fullscreen = enable;
        if (fullscreen) {
            SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN);
        } else {
            SDL_SetWindowFullscreen(sdlWindow, 0);
        }
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
    posX = x;
    posY = y;
}

void Window::resize(int w, int h) {
    SDL_SetWindowSize(sdlWindow, w, h);
    width = w;
    height = h;
}

int Window::getX() const {
    int x, y;
    SDL_GetWindowPosition(sdlWindow, &x, &y);
    return x;
}

int Window::getY() const {
    int x, y;
    SDL_GetWindowPosition(sdlWindow, &x, &y);
    return y;
}

int Window::getWidth() const {
    int w, h;
    SDL_GetWindowSize(sdlWindow, &w, &h);
    return w;
}

int Window::getHeight() const {
    int w, h;
    SDL_GetWindowSize(sdlWindow, &w, &h);
    return h;
}
