#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    ID = createShader(vertexPath, fragmentPath);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

unsigned int Shader::compileShader(GLenum type, const char* sourcePath) {
    std::ifstream file(sourcePath);
    std::stringstream buffer;

    if (file.is_open()) {
        buffer << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << sourcePath << "\"!" << std::endl;
    }
    else {
        std::cerr << "Greska pri citanju fajla sa putanje \"" << sourcePath << "\"!" << std::endl;
        return 0;
    }

    std::string sourceCode = buffer.str();
    const char* code = sourceCode.c_str();

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            std::cerr << "VERTEX sejder ima gresku! Greska:\n" << infoLog << std::endl;
        else if (type == GL_FRAGMENT_SHADER)
            std::cerr << "FRAGMENT sejder ima gresku! Greska:\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int Shader::createShader(const char* vertexPath, const char* fragmentPath) {
    unsigned int program = glCreateProgram();
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Objedinjeni sejder ima gresku! Greska:\n" << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
