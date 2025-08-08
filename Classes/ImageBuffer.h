#pragma once
#include <string>
#include <glad/gl.h>

class ImageBuffer {
public:
    ImageBuffer();
    ~ImageBuffer();

    bool loadFromFile(const std::string& filename);
    void bind() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    GLuint getTextureID() const { return textureID; }

private:
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
    int channels = 0;

    void release();
};
