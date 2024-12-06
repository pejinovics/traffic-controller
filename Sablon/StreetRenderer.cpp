#include "StreetRenderer.h"
#include <GL/glew.h>

StreetRenderer::StreetRenderer(const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {

    shader = new Shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

    texture = loadImageToTexture(texturePath);

    float vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f, 
         1.0f, -1.0f,  1.0f, 0.0f, 
         1.0f,  1.0f,  1.0f, 1.0f, 
        -1.0f,  1.0f,  0.0f, 1.0f  
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

StreetRenderer::~StreetRenderer() {
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    delete shader;
}

void StreetRenderer::render() {
    shader->use();

    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader->unbind();
}
