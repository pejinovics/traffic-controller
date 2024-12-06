#ifndef SEGMENT_H
#define SEGMENT_H

#include <array>
#include <glm/glm.hpp>

class Segment {
private:
    std::array<float, 2> congestion;

public:
    Segment();

    void setCongestion(float left, float right);

    void updateCongestionGradual(bool isGreen, float deltaTime, float greenRate, float redRate);

    float getCongestionLeft() const;
    float getCongestionRight() const;

    glm::vec3 calculateColor() const;
};

#endif // SEGMENT_H
