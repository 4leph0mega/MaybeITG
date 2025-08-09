#pragma once
#include <string>
#include <glad/gl.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();                          // Default simple shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath); // Load from files
    ~Shader();

    void use() const;
    void unuse() const;

    GLuint getProgramID() const { return programID; }

    // Uniform setters
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    GLuint programID = 0;

    bool compileShader(GLuint shader, const std::string& source);
    bool linkProgram(GLuint vertexShader, GLuint fragmentShader);

    std::string readFile(const std::string& path);
};
