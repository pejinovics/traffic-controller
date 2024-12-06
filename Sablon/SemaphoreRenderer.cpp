#include "SemaphoreRenderer.h"
#include <iostream>
#include <random>
#include <glm/gtc/type_ptr.hpp> 

SemaphoreRenderer::SemaphoreRenderer(SemaphoreType type, const std::string& arrowTexturePath)
    : type(type), texture(0), arrowTexture(0), basicVAO(0), arrowVAO(0), VBO(0), basicEBO(0), arrowEBO(0),
    timeAccumulator(0.0f), arrowBlink(false), currentState(RED), textRenderer(textRenderer) {

    texturePath = "semaphore.png";
    vertexShaderPath = "semaphore.vert";
    fragmentShaderPath = "semaphore.frag";
    texture = loadImageToTexture(texturePath);

    if (type == WITH_ARROW && !arrowTexturePath.empty()) {
        arrowTexture = loadImageToTexture(arrowTexturePath);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(20, 40);
    timerDuration = static_cast<float>(dist(gen));
    halfTimer = timerDuration / 2.0f;

    lightStates[0] = 1.0f;
    lightStates[1] = 0.0f; 
    lightStates[2] = 0.0f;
    arrowState = 0.0f; 
    blinkTimer = 0.0f;  

    initialize();
}

SemaphoreRenderer::~SemaphoreRenderer() {
    glDeleteVertexArrays(1, &basicVAO);
    glDeleteVertexArrays(1, &arrowVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &basicEBO);
    glDeleteBuffers(1, &arrowEBO);
    glDeleteTextures(1, &texture);
    if (type == WITH_ARROW) {
        glDeleteTextures(1, &arrowTexture);
    }
    delete shader;
}

float SemaphoreRenderer::getInitialTimer() const {
    return timerDuration; 
}

float SemaphoreRenderer::getTimer() const {
    float remainingTime = 0.0f;

    switch (currentState) {
    case GREEN:
        remainingTime = halfTimer - timeAccumulator; 
        break;
    case RED_YELLOW:
    case YELLOW:
        remainingTime = 3.0f - timeAccumulator; 
        break;
    case RED:
        remainingTime = timerDuration - timeAccumulator;
        break;
    }

    return remainingTime < 0.0f ? 0.0f : remainingTime;
}

glm::vec3 SemaphoreRenderer::getCurrentColor() const {
    if (currentState == GREEN) return glm::vec3(0.0f, 1.0f, 0.0f); 
    if (currentState == YELLOW) return glm::vec3(1.0f, 0.831f, 0.231f);
    if (currentState == RED_YELLOW) return glm::vec3(1.0f, 0.831f, 0.231f);
    return glm::vec3(1.0f, 0.0f, 0.0f); 
}

void SemaphoreRenderer::connectSegment(Segment& segment) {
    connectedSegments.push_back(&segment);
}

void SemaphoreRenderer::updateSegments(float deltaTime, float greenRate, float redRate) {
    bool isGreen = (currentState == GREEN);
    for (Segment* segment : connectedSegments) {
        segment->updateCongestionGradual(isGreen, deltaTime, greenRate, redRate);
    }
}

void SemaphoreRenderer::initialize() {
    initializeVAO();
    shader = new Shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
}

void SemaphoreRenderer::initializeVAO() {
    float vertices[] = {
        -0.0250f,  0.0250f, 0.25f, 0.0f,
         0.0250f,  0.0250f, 0.5f,  0.0f,
         0.0250f,  0.0750f, 0.5f,  1.0f,
        -0.0250f,  0.0750f, 0.25f, 1.0f,

        -0.0250f, -0.0250f, 0.5f,  0.0f,
         0.0250f, -0.0250f, 0.75f, 0.0f,
         0.0250f,  0.0250f, 0.75f, 1.0f,
        -0.0250f,  0.0250f, 0.5f,  1.0f,

        -0.0250f, -0.0750f, 0.75f, 0.0f,
         0.0250f, -0.0750f, 1.0f,  0.0f,
         0.0250f, -0.0250f, 1.0f,  1.0f,
        -0.0250f, -0.0250f, 0.75f, 1.0f,

         0.0250f, -0.0750f, 1.0f,  0.0f, 
         0.0750f, -0.0750f, 1.5f,  0.0f,   
         0.0750f, -0.0250f, 1.5f,  1.0f,   
         0.0250f, -0.0250f, 1.0f,  1.0f   
    };

    unsigned int basicIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8
    };

    unsigned int arrowIndices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12
    };

    glGenVertexArrays(1, &basicVAO);
    glBindVertexArray(basicVAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &basicEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, basicEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(basicIndices), basicIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (type == WITH_ARROW) {
        glGenVertexArrays(1, &arrowVAO);
        glBindVertexArray(arrowVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glGenBuffers(1, &arrowEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arrowEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(arrowIndices), arrowIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void SemaphoreRenderer::updateLights(float deltaTime) {
    timeAccumulator += deltaTime;

    switch (currentState) {
    case RED:
        lightStates[0] = 1.0f; lightStates[1] = 0.0f; lightStates[2] = 0.0f;
        arrowState = 1.0f;
        updateSegments(deltaTime, 0.0f, 0.5f);
        if (timeAccumulator >= timerDuration) {
            currentState = RED_YELLOW;
            timeAccumulator = 0.0f;
        }
        break;
    case RED_YELLOW:
        lightStates[0] = 1.0f; lightStates[1] = 1.0f; lightStates[2] = 0.0f;
        arrowState = 0.0f;
        if (timeAccumulator >= 3.0f) {
            currentState = GREEN;
            timeAccumulator = 0.0f;
        }
        break;
    case GREEN:
        lightStates[0] = 0.0f; lightStates[1] = 0.0f; lightStates[2] = 1.0f;
        arrowState = 0.0f;

        updateSegments(deltaTime, 2.0f, 0.0f);
        if (timeAccumulator >= halfTimer) {
            currentState = YELLOW;
            timeAccumulator = 0.0f;
        }
        break;
    case YELLOW:
        lightStates[0] = 0.0f; lightStates[1] = 1.0f; lightStates[2] = 0.0f;

        blinkTimer += deltaTime;
        if (blinkTimer >= 0.5f) { 
            arrowBlink = !arrowBlink;
            blinkTimer = 0.0f; 
        }
        arrowState = arrowBlink ? 1.0f : 0.0f;

        if (timeAccumulator >= 3.0f) {
            currentState = RED;
            timeAccumulator = 0.0f;
        }
        break;
    }
}

void SemaphoreRenderer::render(float deltaTime, const glm::mat4& modelMatrix) {
    updateLights(deltaTime);

    shader->use();

    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniform3fv(glGetUniformLocation(shader->ID, "uLightStates"), 1, lightStates);

    if (type == WITH_ARROW) {
        glUniform1f(glGetUniformLocation(shader->ID, "uArrowState"), arrowState);
    }

    if (type == BASIC) {
        glBindVertexArray(basicVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader->ID, "uTex"), 0);

        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
    }
    else if (type == WITH_ARROW) {
        glBindVertexArray(arrowVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader->ID, "uTex"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, arrowTexture);
        glUniform1i(glGetUniformLocation(shader->ID, "uArrowTex"), 1);

        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    shader->unbind();
}


