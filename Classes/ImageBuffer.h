class ImageBuffer {
public:
    GLuint textureID = 0; // OpenGL texture handle

    void setTexture(GLuint tex) {
        textureID = tex;
    }

    GLuint getTexture() const {
        return textureID;
    }
};
