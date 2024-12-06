#include "RenderSegment.h"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

void renderSegment(const Segment& segment, const glm::vec2& start, const glm::vec2& end, Shader& shader) {
    glm::vec3 color = segment.calculateColor();

    float vertices[] = {
        start.x, start.y, 0.0f,
        end.x, end.y, 0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    shader.use();
    glUniform3fv(glGetUniformLocation(shader.ID, "uColor"), 1, glm::value_ptr(color));

    glLineWidth(2.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}
