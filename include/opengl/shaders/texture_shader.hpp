#ifndef TEXTURE_SHADER_H
#define TEXTURE_SHADER_H

#include "opengl/shaders/shader.hpp"
#include "types/types.hpp"

struct TextureShader : Shader {

    unsigned int texture_y, texture_u, texture_v;
    textures_t textures = {&texture_y, &texture_u, &texture_v};

    TextureShader(std::string vertexPath, std::string fragmentPath);
};

#endif
