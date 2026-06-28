precision highp float;
varying vec2 TexCoord;

uniform sampler2D texture_y;
uniform sampler2D texture_u;
uniform sampler2D texture_v;

void main(void) {
    float r, g, b, y, u, v;
    y = texture2D(texture_y, TexCoord).r;
    u = texture2D(texture_u, TexCoord).r;
    v = texture2D(texture_v, TexCoord).r;

    y = 1.1643 * (y - 0.0625);
    u = u - 0.5;
    v = v - 0.5;

    r = y + 1.5958 * v;
    g = y - 0.39173 * u - 0.81290 * v;
    b = y + 2.017 * u;
    gl_FragColor = vec4(r, g, b, 1.0);
}
