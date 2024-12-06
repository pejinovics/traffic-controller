#pragma once
#ifndef RENDER_SEGMENT_H
#define RENDER_SEGMENT_H

#include "Segment.h"
#include "Shader.h"
#include <glm/glm.hpp>

void renderSegment(const Segment& segment, const glm::vec2& start, const glm::vec2& end, Shader& shader);

#endif

