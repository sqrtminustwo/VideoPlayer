#include "opengl/shaders/texture_shader.hpp"
#include <glad.h>
#include <GLFW/glfw3.h>

using namespace std;

TextureShader::TextureShader(string vertexPath, string fragmentPath)
    : Shader(vertexPath, fragmentPath) {
    use();
    setInt("texture_y", 0);
    setInt("texture_u", 1);
    setInt("texture_v", 2);

    for (int i = 0; i < textures.size(); i++) glGenTextures(1, textures.at(i));
}
