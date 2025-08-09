#include "RenderTexture.h"
#include <iostream>

RenderTexture::RenderTexture(int w, int h)
    : width(w), height(h), bgColor(0, 0, 0, 1) {
    initFBO();
}

RenderTexture::~RenderTexture() {
    cleanupFBO();
}

void RenderTexture::initFBO() {
    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture->getTextureID(), 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete: " << status << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderTexture::cleanupFBO() {
    if (fbo) {
        glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }
}

void RenderTexture::setBackgroundColor(float r, float g, float b, float a) {
    bgColor = {r, g, b, a};
}

glm::vec4 RenderTexture::getBackgroundColor() const {
    return bgColor;
}

void RenderTexture::addSprite(const std::shared_ptr<Sprite>& sprite) {
    sprites.push_back(sprite);
}

void RenderTexture::removeSprite(const std::shared_ptr<Sprite>& sprite) {
    sprites.erase(std::remove(sprites.begin(), sprites.end(), sprite), sprites.end());
}

void RenderTexture::setGlobalShader(const std::shared_ptr<Shader>& shader) {
    globalShader = shader;
}

std::shared_ptr<Shader> RenderTexture::getGlobalShader() const {
    return globalShader;
}

void RenderTexture::render() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);

    glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (globalShader) {
        globalShader->use();
    }

    for (auto& sprite : sprites) {
        sprite->draw(globalShader.get());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTexture::SetBackgroundColor(float r, float g, float b, float a) {
    bgColor[0] = r;
    bgColor[1] = g;
    bgColor[2] = b;
    bgColor[3] = a;
}

void RenderTexture::clearSprites() {
    sprites.clear();
}

ImageBuffer* RenderTexture::GetOutputImageBuffer() {
    return outputTexture.get();
}

std::shared_ptr<ImageBuffer> RenderTexture::getOutputImageBuffer() const {
    return outputTexture;
}

int RenderTexture::getWidth() const { return width; }
int RenderTexture::getHeight() const { return height; }
