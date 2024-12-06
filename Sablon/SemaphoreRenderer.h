#ifndef SEMAPHORE_RENDERER_H
#define SEMAPHORE_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"
#include "Texture.h"
#include "Segment.h"
#include <vector>
#include "TextRenderer.h"

enum SemaphoreType { BASIC, WITH_ARROW };

class SemaphoreRenderer {
public:
    SemaphoreRenderer(SemaphoreType type, const std::string& arrowTexturePath = "");
    ~SemaphoreRenderer();

    void connectSegment(Segment& segment);
    void updateSegments(float deltaTime, float greenRate, float redRate);
    void render(float deltaTime, const glm::mat4& modelMatrix);
    void updateLights(float deltaTime);
    float getInitialTimer() const;      
    float getTimer() const;        
    glm::vec3 getCurrentColor() const;

private:
    std::vector<Segment*> connectedSegments;
    SemaphoreType type;
    unsigned int basicVAO, arrowVAO, VBO, basicEBO, arrowEBO;
    unsigned int texture, arrowTexture;
    Shader* shader;
    float lightStates[3];
    float arrowState;
    float timeAccumulator;
    float blinkTimer;
    float timerDuration;
    float halfTimer;
    bool arrowBlink;
    TextRenderer* textRenderer;
    enum SemaforState { RED, RED_YELLOW, GREEN, YELLOW };
    SemaforState currentState;
    std::string texturePath;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;

    void initializeVAO();
    void initialize();
};

#endif
