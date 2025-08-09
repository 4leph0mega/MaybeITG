#pragma once
#include <vector>
#include <memory>
#include <glad/gl.h>
#include <glm/vec4.hpp>
#include "ImageBuffer.h"
#include "Sprite.h"
#include "Shader.h"

class RenderTexture {
public:
    RenderTexture(int width, int height);
    ~RenderTexture();

    void setBackgroundColor(float r, float g, float b, float a);
    glm::vec4 getBackgroundColor() const;

    void addSprite(const std::shared_ptr<Sprite>& sprite);
    void removeSprite(const std::shared_ptr<Sprite>& sprite);

    void setGlobalShader(const std::shared_ptr<Shader>& shader);
    std::shared_ptr<Shader> getGlobalShader() const;

    void SetBackgroundColor(float r, float g, float b, float a);
    void clearSprites();
    ImageBuffer* GetOutputImageBuffer();
    void render();
    

    std::shared_ptr<ImageBuffer> getOutputImageBuffer() const;

    int getWidth() const;
    int getHeight() const;

private:
    int width, height;
    glm::vec4 bgColor;

    std::vector<std::shared_ptr<Sprite>> sprites;
    std::shared_ptr<Shader> globalShader;

    std::shared_ptr<ImageBuffer> outputTexture;

    GLuint fbo = 0;

    void initFBO();
    void cleanupFBO();
};
