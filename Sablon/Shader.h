#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>   

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    void unbind();
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setMat4(const std::string& name, const glm::mat4& mat);

private:
    unsigned int compileShader(GLenum type, const char* sourcePath);
    unsigned int createShader(const char* vertexPath, const char* fragmentPath);
};

#endif
