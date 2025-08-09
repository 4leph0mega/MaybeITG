#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader() {
    // Default simple shader source code (textured quad)
    const char* defaultVertexSrc = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        uniform mat4 model;
        uniform mat4 projection;

        void main() {
            gl_Position = projection * model * vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* defaultFragmentSrc = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D texture1;

        void main() {
            FragColor = texture(texture1, TexCoord);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!compileShader(vertexShader, defaultVertexSrc)) {
        std::cerr << "Default vertex shader compilation failed\n";
        glDeleteShader(vertexShader);
        return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(fragmentShader, defaultFragmentSrc)) {
        std::cerr << "Default fragment shader compilation failed\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    programID = glCreateProgram();
    if (!linkProgram(vertexShader, fragmentShader)) {
        std::cerr << "Default shader program linking failed\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programID);
        programID = 0;
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "Shader source empty!\n";
        return;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!compileShader(vertexShader, vertexCode)) {
        std::cerr << "Vertex shader compilation failed\n";
        glDeleteShader(vertexShader);
        return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(fragmentShader, fragmentCode)) {
        std::cerr << "Fragment shader compilation failed\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    programID = glCreateProgram();
    if (!linkProgram(vertexShader, fragmentShader)) {
        std::cerr << "Shader program linking failed\n";
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programID);
        programID = 0;
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

void Shader::use() const {
    if (programID != 0)
        glUseProgram(programID);
}

void Shader::unuse() const {
    glUseProgram(0);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

bool Shader::compileShader(GLuint shader, const std::string& source) {
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

bool Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    int success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Program linking error:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
