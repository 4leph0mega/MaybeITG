#include "ImageBuffer.h"
#include <stb_image/stb_image.h>
#include <iostream>

ImageBuffer::ImageBuffer() {
    glGenTextures(1, &textureID);
}

ImageBuffer::~ImageBuffer() {
    release();
}

void ImageBuffer::release() {
    if (textureID) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

bool ImageBuffer::loadFromFile(const std::string& filename) {
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load image: " << filename << "\n";
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload to GPU
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return true;
}

void ImageBuffer::bind() const {
    glBindTexture(GL_TEXTURE_2D, textureID);
}
