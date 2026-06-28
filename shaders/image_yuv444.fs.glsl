#version 330 core
#extension GL_ARB_shading_language_420pack : require

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture_y;
uniform sampler2D texture_u;
uniform sampler2D texture_v;

void main() {
    float y = texture(texture_y, TexCoord).r;
    float u = texture(texture_u, TexCoord).r - 0.5;
    float v = texture(texture_v, TexCoord).r - 0.5;

    float r = y + 1.402 * v;
    float g = y - 0.344136 * u - 0.714136 * v;
    float b = y + 1.772 * u;

    FragColor = vec4(r, g, b, 1.0);
}
