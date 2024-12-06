#include "CityImageRenderer.h"
#include <iostream>
#include "Texture.h" 

CityImageRenderer::CityImageRenderer(const std::string& texturePath, const std::string& vertexShader, const std::string& fragmentShader)
    : shader(vertexShader.c_str(), fragmentShader.c_str()) {

    texture = loadImageToTexture(texturePath);

    initializeRectangle();

    glm::mat4 projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
    shader.use();
    shader.setMat4("projection", projection);
}

CityImageRenderer::~CityImageRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void CityImageRenderer::initializeRectangle() {
    float vertices[] = {
        0.8f,  1.0f,  0.0f, 0.0f, 1.0f,  
        0.8f,  0.9f,  0.0f, 0.0f, 0.0f,  
        1.0f,  0.9f,  0.0f, 1.0f, 0.0f, 
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f  
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3  
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void CityImageRenderer::render() {
    shader.use();
    shader.setInt("texture1", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
