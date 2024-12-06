#pragma once
#ifndef STREET_RENDERER_H
#define STREET_RENDERER_H

#include "Shader.h"
#include "Texture.h"

class StreetRenderer {
public:
    StreetRenderer(const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~StreetRenderer();

    void render();

private:
    unsigned int VAO, VBO, EBO; 
    unsigned int texture;      
    Shader* shader;             
};

#endif

