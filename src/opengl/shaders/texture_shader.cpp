#include "opengl/shaders/texture_shader.hpp"
#include <glad.h>
#include <GLFW/glfw3.h>

TextureShader::TextureShader(std::string vertexPath, std::string fragmentPath)
    : Shader(vertexPath, fragmentPath) {
    use();
    glUniform1i(glGetUniformLocation(ID, "texture_y"), 0);
    glUniform1i(glGetUniformLocation(ID, "texture_u"), 1);
    glUniform1i(glGetUniformLocation(ID, "texture_v"), 2);

    for (int i = 0; i < textures.size(); i++) glGenTextures(1, textures.at(i));
}
