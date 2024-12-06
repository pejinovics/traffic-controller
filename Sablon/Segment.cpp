#include "Segment.h"
#include <glm/vec3.hpp>
#include <algorithm>

glm::vec3 linearMix(const glm::vec3& start, const glm::vec3& end, float factor) {
    return start + factor * (end - start);
}
float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

Segment::Segment() {
    congestion.fill(0.0f);
}

void Segment::setCongestion(float left, float right) {
    congestion[0] = clamp(left, 0.0f, 1.0f);
    congestion[1] = clamp(right, 0.0f, 1.0f); 
}

void Segment::updateCongestionGradual(bool isGreen, float deltaTime, float greenRate, float redRate) {
    float rate = isGreen ? greenRate : redRate;

    for (auto& c : congestion) {
        float target = isGreen ? 0.0f : 1.0f; 
        c += (target - c) * rate * deltaTime; 
        c = clamp(c, 0.0f, 1.0f);            
    }
}

float Segment::getCongestionLeft() const {
    return congestion[0];
}

float Segment::getCongestionRight() const {
    return congestion[1];
}

glm::vec3 Segment::calculateColor() const {
    float avgCongestion = (congestion[0] + congestion[1]) / 2.0f;
    return linearMix(glm::vec3(0.0f, 1.0f, 0.0f), 
        glm::vec3(1.0f, 0.0f, 0.0f), 
        avgCongestion);
}
