#ifndef CITY_IMAGE_RENDERER_H
#define CITY_IMAGE_RENDERER_H

#include <string>
#include <glm/glm.hpp>
#include "Shader.h"

class CityImageRenderer {
public:
    CityImageRenderer(const std::string& texturePath, const std::string& vertexShader, const std::string& fragmentShader);
    ~CityImageRenderer();

    void render();

private:
    unsigned int VAO, VBO, EBO, texture;
    Shader shader;

    void initializeRectangle();
};

#endif
