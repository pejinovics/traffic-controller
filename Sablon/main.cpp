#include "SemaphoreRenderer.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "StreetRenderer.h"
#include "RenderSegment.h"
#include "Segment.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "TextRenderer.h"
#include "CityImageRenderer.h"
#include <thread> 
#include <chrono>

struct SegmentData {
    Segment* segment;
    glm::vec2 start;
    glm::vec2 end;
};

struct Street {
    std::string name;
    float x1, y1, x2, y2; 
};

bool isCursorInStreet(float cursorX, float cursorY, const Street& street) {
    return cursorX >= street.x1 && cursorX <= street.x2 &&
        cursorY >= street.y1 && cursorY <= street.y2;
}

void displayStreetName(glm::vec2 mousePos, const std::vector<Street>& streets, TextRenderer& textRenderer) {
    for (const auto& street : streets) {
        if (mousePos.x >= street.x1 && mousePos.x <= street.x2 &&
            mousePos.y >= street.y1 && mousePos.y <= street.y2) {
            textRenderer.renderText(street.name, 25.0f, 25.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
            break;
        }
    }
}


void renderSemaphore(SemaphoreRenderer& semaphore, float deltaTime, glm::vec3 translation, float rotationAngle);
void renderSemaphoreInfo(
    SemaphoreRenderer& semaphore,
    const glm::mat4& model,
    TextRenderer& textRenderer,
    glm::vec3 position, float scale);
void renderFullSemaphore(
    SemaphoreRenderer& semaphore,
    float deltaTime,
    glm::vec3 translation,
    float rotationAngle,
    TextRenderer& textRenderer,
    glm::vec3 infoPosition,
    float infoScale
);
void processMouseInput(GLFWwindow* window, const glm::vec2& mousePos, const std::vector<SegmentData>& segments);
bool isMouseOverSegment(const glm::vec2& mousePos, const glm::vec2& start, const glm::vec2& end);

float greenRate = 5.0f;
float redRate = 3.0f;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        greenRate = std::min(greenRate + 5.0f, 50.0f); 
        redRate = std::min(redRate + 3.0f, 30.0f);     
    }
    else if (yoffset < 0) {
        greenRate = std::max(greenRate - 1.5f, 2.0f);  
        redRate = std::max(redRate - 0.5f, 1.0f);      
    }

    std::cout << "Scroll detected: greenRate = " << greenRate << ", redRate = " << redRate << std::endl;
}


int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1200, 800, "Traffic controller", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback); 
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }


    TextRenderer tr("LiberationSans-Regular.ttf");

    TextRenderer trstreets("LiberationSans-Regular.ttf");

    StreetRenderer streetRenderer("street.png", "street.vert", "street.frag");

    CityImageRenderer cityRenderer("name.png", "cityImage.vert", "cityImage.frag");

    SemaphoreRenderer semaphoreFirstTopLeft(BASIC, "");
    SemaphoreRenderer semaphoreFirstBottomRight(BASIC, "");
    SemaphoreRenderer semaphoreFirstTopRight(WITH_ARROW, "semaphore_arrow.png");
    SemaphoreRenderer semaphoreFirstBottomLeft(WITH_ARROW, "semaphore_arrow.png");

    SemaphoreRenderer semaphoreSecondTopLeft(BASIC, "");
    SemaphoreRenderer semaphoreSecondTopRight(WITH_ARROW, "semaphore_arrow.png");
    SemaphoreRenderer semaphoreSecondBottomLeft(WITH_ARROW,  "semaphore_arrow.png");
    SemaphoreRenderer semaphoreSecondBottomRight(BASIC, "");

    SemaphoreRenderer semaphoreThirdTopLeft(BASIC, "");
    SemaphoreRenderer semaphoreThirdTopRight(WITH_ARROW,  "semaphore_arrow.png");
    SemaphoreRenderer semaphoreThirdBottomLeft(WITH_ARROW, "semaphore_arrow.png");
    SemaphoreRenderer semaphoreThirdBottomRight(BASIC,  "");


    Segment segmentFirstTopLeft, segmentFirstBottomRight, segmentSecondTopLeft,
        segmentSecondBottom, segmentThirdTopLeft, segmentThirdBottomRight,
        segmentFirstBottomLeft1, segmentFirstBottomLeft2, segmentSecondBottomLeft1, segmentSecondBottomLeft2, segmentThirdBottomLeft1, segmentThirdBottomLeft2,
        segmentFirstTopRight1, segmentFirstTopRight2, segmentSecondTopRight1, segmentSecondTopRight2, segmentThirdTopRight1, segmentThirdTopRight2;

    segmentFirstTopLeft.setCongestion(0.5f, 0.6f); 
    segmentFirstBottomRight.setCongestion(0.7f, 0.8f);
    segmentSecondTopLeft.setCongestion(0.5f, 0.6f); 
    segmentSecondBottom.setCongestion(0.7f, 0.8f);
    segmentThirdTopLeft.setCongestion(0.5f, 0.6f); 
    segmentThirdBottomRight.setCongestion(0.7f, 0.8f);

    segmentFirstBottomLeft1.setCongestion(0.7f, 0.8f);
    segmentFirstBottomLeft2.setCongestion(0.7f, 0.8f);
    segmentSecondBottomLeft1.setCongestion(0.7f, 0.8f);
    segmentSecondBottomLeft2.setCongestion(0.7f, 0.8f);
    segmentThirdBottomLeft1.setCongestion(0.7f, 0.8f);
    segmentThirdBottomLeft2.setCongestion(0.7f, 0.8f);

    segmentFirstTopRight1.setCongestion(0.7f, 0.8f);
    segmentFirstTopRight2.setCongestion(0.7f, 0.8f);
    segmentSecondTopRight1.setCongestion(0.7f, 0.8f);
    segmentSecondTopRight2.setCongestion(0.7f, 0.8f);
    segmentThirdTopRight1.setCongestion(0.7f, 0.8f);
    segmentThirdTopRight2.setCongestion(0.7f, 0.8f);

    semaphoreFirstTopLeft.connectSegment(segmentFirstTopLeft);
    semaphoreFirstBottomRight.connectSegment(segmentFirstBottomRight);

    semaphoreSecondTopLeft.connectSegment(segmentSecondTopLeft);
    semaphoreSecondBottomRight.connectSegment(segmentSecondBottom);

    semaphoreThirdTopLeft.connectSegment(segmentThirdTopLeft);
    semaphoreThirdBottomRight.connectSegment(segmentThirdBottomRight);

    semaphoreFirstBottomLeft.connectSegment(segmentFirstBottomLeft1);
    semaphoreFirstBottomLeft.connectSegment(segmentFirstBottomLeft2);

    semaphoreSecondBottomLeft.connectSegment(segmentSecondBottomLeft1);
    semaphoreSecondBottomLeft.connectSegment(segmentSecondBottomLeft2);

    semaphoreThirdBottomLeft.connectSegment(segmentThirdBottomLeft1);
    semaphoreThirdBottomLeft.connectSegment(segmentThirdBottomLeft2);

    semaphoreFirstTopRight.connectSegment(segmentFirstTopRight1);
    semaphoreFirstTopRight.connectSegment(segmentFirstTopRight2);

    semaphoreSecondTopRight.connectSegment(segmentSecondTopRight1);
    semaphoreSecondTopRight.connectSegment(segmentSecondTopRight2);

    semaphoreThirdTopRight.connectSegment(segmentThirdTopRight1);
    semaphoreThirdTopRight.connectSegment(segmentThirdTopRight2);


    Shader segmentShader("segment.vert", "segment.frag");

    glm::vec2 segmentFirstTopLeftStart(-0.505f, 0.31f), segmentFirstTopLeftEnd(-0.505f, -0.83f);
    glm::vec2 segmentFirstBottomRightStart(-0.49f, 0.31f), segmentFirstBottomRightEnd(-0.49f, -0.83f);
    glm::vec2 segmentSecondTopLeftStart(-0.195f, 0.31f), segmentSecondTopLeftEnd(-0.195f, -0.83f);
    glm::vec2 segmentSecondBottomRightStart(-0.18f, 0.31f), segmentSecondBottomRightEnd(-0.18, -0.83f);
    glm::vec2 segmentThirdTopLeftStart(0.115, 0.31f), segmentThirdTopLeftEnd(0.115, -0.83f);
    glm::vec2 segmentThirdBottomRightStart(0.13, 0.31f), segmentThirdBottomRightEnd(0.13, -0.83f);

    glm::vec2 segmentFirstBottomLeftStart1(-0.87, -0.24f), segmentFirstBottomLeftEnd1(-0.37, -0.24f);
    glm::vec2 segmentFirstBottomLeftStart2(-0.87, -0.21f), segmentFirstBottomLeftEnd2(-0.37, -0.21f);

    glm::vec2 segmentSecondBottomLeftStart1(-0.37, -0.24f), segmentSecondBottomLeftEnd1(0.00, -0.24f);
    glm::vec2 segmentSecondBottomLeftStart2(-0.37, -0.21f), segmentSecondBottomLeftEnd2(0.00, -0.21f);

    glm::vec2 segmentThirdBottomLeftStart1(0.00, -0.24f), segmentThirdBottomLeftEnd1(0.87, -0.24f);
    glm::vec2 segmentThirdBottomLeftStart2(0.00, -0.21f), segmentThirdBottomLeftEnd2(0.87, -0.21f);

    glm::vec2 segmentFirstTopRightStart1(-0.87, -0.18f), segmentFirstTopRightEnd1(-0.37, -0.18f);
    glm::vec2 segmentFirstTopRightStart2(-0.87, -0.15f), segmentFirstTopRightEnd2(-0.37, -0.15f);

    glm::vec2 segmentSecondTopRightStart1(-0.37, -0.18f), segmentSecondTopRightEnd1(0.00, -0.18f);
    glm::vec2 segmentSecondTopRightStart2(-0.37, -0.15f), segmentSecondTopRightEnd2(0.00, -0.15f);

    glm::vec2 segmentThirdTopRightStart1(0.00, -0.18f), segmentThirdTopRightEnd1(0.87, -0.18f);
    glm::vec2 segmentThirdTopRightStart2(0.00, -0.15f), segmentThirdTopRightEnd2(0.87, -0.15f);

    std::vector<SegmentData> allSegments = {
    { &segmentFirstTopLeft, segmentFirstTopLeftStart, segmentFirstTopLeftEnd },
    { &segmentFirstBottomRight, segmentFirstBottomRightStart, segmentFirstBottomRightEnd },
    { &segmentSecondTopLeft, segmentSecondTopLeftStart, segmentSecondTopLeftEnd },
    { &segmentSecondBottom, segmentSecondBottomRightStart, segmentSecondBottomRightEnd },
    { &segmentThirdTopLeft, segmentThirdTopLeftStart, segmentThirdTopLeftEnd },
    { &segmentThirdBottomRight, segmentThirdBottomRightStart, segmentThirdBottomRightEnd },
    { &segmentFirstBottomLeft1, segmentFirstBottomLeftStart1, segmentFirstBottomLeftEnd1 },
    { &segmentFirstBottomLeft2, segmentFirstBottomLeftStart2, segmentFirstBottomLeftEnd2 },
    { &segmentSecondBottomLeft1, segmentSecondBottomLeftStart1, segmentSecondBottomLeftEnd1 },
    { &segmentSecondBottomLeft2, segmentSecondBottomLeftStart2, segmentSecondBottomLeftEnd2 },
    { &segmentThirdBottomLeft1, segmentThirdBottomLeftStart1, segmentThirdBottomLeftEnd1 },
    { &segmentThirdBottomLeft2, segmentThirdBottomLeftStart2, segmentThirdBottomLeftEnd2 },
    { &segmentFirstTopRight1, segmentFirstTopRightStart1, segmentFirstTopRightEnd1 },
    { &segmentFirstTopRight2, segmentFirstTopRightStart2, segmentFirstTopRightEnd2 },
    { &segmentSecondTopRight1, segmentSecondTopRightStart1, segmentSecondTopRightEnd1 },
    { &segmentSecondTopRight2, segmentSecondTopRightStart2, segmentSecondTopRightEnd2 },
    { &segmentThirdTopRight1, segmentThirdTopRightStart1, segmentThirdTopRightEnd1 },
    { &segmentThirdTopRight2, segmentThirdTopRightStart2, segmentThirdTopRightEnd2 }
    };

    std::vector<Street> streets = {
    {"Bulevar", -0.87f, -0.24f, 0.87f, -0.15f},
    {"Cetvrta ulica", -0.505f, -0.83f, -0.49, -0.24f},
    {"Cetvrta ulica", -0.505f, -0.15, -0.49, 0.31f},
    {"Peta ulica", -0.195f, -0.83f, -0.18f, -0.24f},
    {"Peta ulica", -0.195f, -0.15f, -0.18f, 0.31f},
    {"Peta ulica", -0.195f, 0.35f, -0.18f, 0.85f},
    {"Sesta ulica", 0.115f, -0.83f, 0.13f, -0.24f},
    {"Sesta ulica", 0.115f, -0.15f, 0.13f, 0.31f},
    {"Druga ulica", -0.87f, 0.31f, 0.13f, 0.35f},
    {"Prva ulica", -0.71f, 0.35f, -0.69f, 0.85f},
    {"Druga ulica", -0.18f, -0.60f, 0.87f, -0.56f},
    {"Treca ulica", -0.18f, -0.83f, 0.87f, -0.79f},
    };

    const double TARGET_FRAME_TIME = 1.0 / 60.0;
    double previousTime = glfwGetTime();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - previousTime;
        float deltaTime = static_cast<float>(currentTime - previousTime);

        if (elapsedTime < TARGET_FRAME_TIME) {
            glfwWaitEventsTimeout(TARGET_FRAME_TIME - elapsedTime);
            deltaTime = static_cast<float>(currentTime - previousTime);
            elapsedTime = currentTime - previousTime;
            continue;
        }

        previousTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);

        streetRenderer.render();
        cityRenderer.render();

        renderFullSemaphore(semaphoreFirstTopLeft, deltaTime, glm::vec3(-0.54f, 0.05f, 0.0f), -180.0f, tr, glm::vec3(315.0f, 420.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreFirstBottomRight, deltaTime, glm::vec3(-0.44f, -0.42f, 0.0f), 0.0f, tr, glm::vec3(425.0f, 320.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreFirstBottomLeft, deltaTime, glm::vec3(-0.59f, -0.29f, 0.0f), -90.0f, tr, glm::vec3(315.0f, 285.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreFirstTopRight, deltaTime, glm::vec3(-0.39f, -0.09f, 0.0f), 90.0f, tr, glm::vec3(425.0f, 450.0f, 0.0f), 0.28f);

        renderFullSemaphore(semaphoreSecondTopLeft, deltaTime, glm::vec3(-0.23f, 0.05f, 0.0f), -180.0f, tr, glm::vec3(565.0f, 420.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreSecondBottomRight, deltaTime, glm::vec3(-0.13f, -0.42f, 0.0f), 0.0f, tr, glm::vec3(675.0f, 320.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreSecondBottomLeft, deltaTime, glm::vec3(-0.28f, -0.29f, 0.0f), -90.0f, tr, glm::vec3(565.0f, 285.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreSecondTopRight, deltaTime, glm::vec3(-0.08f, -0.09f, 0.0f), 90.0f, tr, glm::vec3(680.0f, 450.0f, 0.0f), 0.28f);

        renderFullSemaphore(semaphoreThirdTopLeft, deltaTime, glm::vec3(0.08f, 0.05f, 0.0f), -180.0f, tr, glm::vec3(810.0f, 420.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreThirdBottomRight, deltaTime, glm::vec3(0.18f, -0.42f, 0.0f), 0.0f, tr, glm::vec3(920.0f, 320.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreThirdBottomLeft, deltaTime, glm::vec3(0.03f, -0.29f, 0.0f), -90.0f, tr, glm::vec3(810.0f, 285.0f, 0.0f), 0.28f);
        renderFullSemaphore(semaphoreThirdTopRight, deltaTime, glm::vec3(0.23f, -0.09f, 0.0f), 90.0f, tr, glm::vec3(920.0f, 450.0f, 0.0f), 0.28f);

        for (const auto& data : allSegments) {
            renderSegment(*data.segment, data.start, data.end, segmentShader);
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        glm::vec2 mousePos = glm::vec2(
            (mouseX / 600.0f) - 1.0f,
            1.0f - (mouseY / 400.0f)
        );

        displayStreetName(mousePos, streets, trstreets);
        processMouseInput(window, mousePos, allSegments);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
void renderSemaphore(SemaphoreRenderer& semaphore, float deltaTime, glm::vec3 translation, float rotationAngle) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translation);
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    semaphore.render(deltaTime, model);
}

void renderSemaphoreInfo(
    SemaphoreRenderer& semaphore,
    const glm::mat4& model,
    TextRenderer& textRenderer,
    glm::vec3 position, 
    float scale        
) {
    std::string initialTimerText = "Tajmer: " + std::to_string(static_cast<int>(semaphore.getInitialTimer()));
    textRenderer.renderText(
        initialTimerText,
        position.x,       
        position.y,      
        scale,            
        glm::vec3(0.0f, 0.0f, 1.0f) 
    );

    std::string remainingTimeText = "Preostalo: " + std::to_string(static_cast<int>(semaphore.getTimer()));
    glm::vec3 activeColor = semaphore.getCurrentColor();
    textRenderer.renderText(
        remainingTimeText,
        position.x,        
        position.y - 20.0f, 
        scale,             
        activeColor       
    );
}

void renderFullSemaphore(
    SemaphoreRenderer& semaphore,
    float deltaTime,
    glm::vec3 translation,      
    float rotationAngle,           
    TextRenderer& textRenderer,
    glm::vec3 infoPosition,        
    float infoScale         
) {
    renderSemaphore(semaphore, deltaTime, translation, rotationAngle);
    renderSemaphoreInfo(semaphore, glm::mat4(1.0f), textRenderer, infoPosition, infoScale);
}

void processMouseInput(GLFWwindow* window, const glm::vec2& mousePos, const std::vector<SegmentData>& segments) {
    for (const auto& data : segments) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (isMouseOverSegment(mousePos, data.start, data.end)) {
                data.segment->setCongestion(
                    std::min(data.segment->getCongestionLeft() + 0.1f, 1.0f),
                    std::min(data.segment->getCongestionRight() + 0.1f, 1.0f)
                );
            }
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            if (isMouseOverSegment(mousePos, data.start, data.end)) {
                data.segment->setCongestion(
                    std::max(data.segment->getCongestionLeft() - 0.1f, 0.0f),
                    std::max(data.segment->getCongestionRight() - 0.1f, 0.0f)
                );
            }
        }
    }
}

float length2(const glm::vec2& vec) {
    return glm::dot(vec, vec); 
}

bool isMouseOverSegment(const glm::vec2& mousePos, const glm::vec2& start, const glm::vec2& end) {
    const float tolerance = 0.01f; 
    glm::vec2 segmentDir = end - start;
    glm::vec2 mouseDir = mousePos - start;

    float dot = glm::dot(mouseDir, segmentDir);
    float segmentLengthSquared = glm::dot(segmentDir, segmentDir);

    if (dot < 0.0f || dot > segmentLengthSquared) {
        return false;
    }

    float distanceSquared = length2(mouseDir - segmentDir * (dot / segmentLengthSquared));
    return distanceSquared <= tolerance * tolerance;
}
