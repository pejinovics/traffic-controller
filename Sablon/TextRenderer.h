#ifndef TEXT_HANDLER_H
#define TEXT_HANDLER_H

#include "shader.h"
#include <GL/glew.h>
#include <map>
#include <glm/glm.hpp>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H  

class TextRenderer {
public:
    struct Character {
        unsigned int TextureID;
        glm::ivec2   Size;
        glm::ivec2   Bearing;
        unsigned int Advance;
    };
    unsigned int VAO, VBO;
    Shader shader;
    std::map<char, Character> Characters;

    TextRenderer(const char* fontPath);
    ~TextRenderer();
    int loadFont(const char* fontPath);
    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

    static const char* VERT_SHADER;
    static const char* FRAG_SHADER;
    static const char* TEXTURE;
};

#endif 