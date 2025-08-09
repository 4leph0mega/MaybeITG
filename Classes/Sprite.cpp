#include "Sprite.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

GLuint Sprite::quadVAO = 0;
GLuint Sprite::quadVBO = 0;

Sprite::Sprite() {
    if (quadVAO == 0)
        initQuad();
}

Sprite::Sprite(std::shared_ptr<ImageBuffer> tex) : texture(std::move(tex)) {
    if (quadVAO == 0)
        initQuad();
}

void Sprite::initQuad() {
    float vertices[] = {
        // positions    // tex coords
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top-right
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // top-left
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // tex coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void Sprite::setTexture(std::shared_ptr<ImageBuffer> tex) {
    texture = std::move(tex);
}

std::shared_ptr<ImageBuffer> Sprite::getTexture() const {
    return texture;
}

void Sprite::setPosition(float x, float y, float z) {
    position = {x, y, z};
    transformDirty = true;
}

void Sprite::move(float dx, float dy, float dz) {
    position += glm::vec3(dx, dy, dz);
    transformDirty = true;
}

void Sprite::setRotation(float angleRadians, const glm::vec3& axis) {
    rotationAngle = angleRadians;
    rotationAxis = axis;
    transformDirty = true;
}

void Sprite::rotate(float dAngleRadians, const glm::vec3& dAxis) {
    rotationAngle += dAngleRadians;
    rotationAxis = glm::normalize(rotationAxis + dAxis);
    transformDirty = true;
}

void Sprite::setSkew(float x, float y) {
    skewX = x;
    skewY = y;
    transformDirty = true;
}

void Sprite::skew(float dx, float dy) {
    skewX += dx;
    skewY += dy;
    transformDirty = true;
}

void Sprite::setScale(float sx, float sy) {
    scale = {sx, sy};
    transformDirty = true;
}

void Sprite::scaleBy(float sx, float sy) {
    scale.x *= sx;
    scale.y *= sy;
    transformDirty = true;
}

void Sprite::updateTransform() const {
    if (!transformDirty) return;

    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);

    glm::mat4 S(1.0f);
    S[1][0] = skewX; // skewX on Y
    S[0][1] = skewY; // skewY on X

    glm::mat4 Sc = glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));

    transform = T * R * S * Sc;
    transformDirty = false;
}

glm::mat4 Sprite::getTransform() const {
    updateTransform();
    return transform;
}

void Sprite::setShader(std::shared_ptr<Shader> s) {
    shader = std::move(s);
}

std::shared_ptr<Shader> Sprite::getShader() const {
    return shader;
}

void Sprite::draw(Shader* overrideShader) {
    Shader* activeShader = overrideShader ? overrideShader : shader.get();
    if (!activeShader) {
        std::cerr << "No shader to draw sprite!\n";
        return;
    }

    activeShader->use();

    activeShader->setMat4("u_ModelMatrix", getTransform());

    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
        activeShader->setInt("u_Texture", 0);
    }

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
