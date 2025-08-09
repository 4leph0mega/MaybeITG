#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "ImageBuffer.h"
#include "Shader.h"

class Sprite : public std::enable_shared_from_this<Sprite> {
public:
    Sprite();
    explicit Sprite(std::shared_ptr<ImageBuffer> texture);

    void setTexture(std::shared_ptr<ImageBuffer> texture);
    std::shared_ptr<ImageBuffer> getTexture() const;

    void setPosition(float x, float y, float z = 0.0f);
    void move(float dx, float dy, float dz = 0.0f);

    void setRotation(float angleRadians, const glm::vec3& axis);
    void rotate(float dAngleRadians, const glm::vec3& dAxis);

    void setSkew(float skewX, float skewY);
    void skew(float dSkewX, float dSkewY);

    void setScale(float sx, float sy);
    void scaleBy(float sx, float sy);

    glm::mat4 getTransform() const;

    void setShader(std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> getShader() const;

    void draw(Shader* overrideShader = nullptr);

private:
    std::shared_ptr<ImageBuffer> texture;

    glm::vec3 position = {0, 0, 0};
    float rotationAngle = 0.0f;
    glm::vec3 rotationAxis = {0, 0, 1};

    float skewX = 0.0f;
    float skewY = 0.0f;

    glm::vec2 scale = {1.0f, 1.0f};

    std::shared_ptr<Shader> shader;

    mutable glm::mat4 transform;
    mutable bool transformDirty = true;

    void updateTransform() const;

    // VAO/VBO for quad rendering (static shared across all sprites)
    static GLuint quadVAO;
    static GLuint quadVBO;
    static void initQuad();
};
