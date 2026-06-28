// https://gist.github.com/crearo/798412489698e749c17e572e74496e1b

/** A fragment shader to convert YUV420P to RGB.
  * Input textures Y - is a block of size w*h.
  * texture U is of size w/2*h/2.
  * texture V is of size w/2*h/2.
  * In this case, the layout looks like the following :
  * __________
  * |        |
  * |   Y    | size = w*h
  * |        |
  * |________|
  * |____U___|size = w*(h/4)
  * |____V___|size = w*(h/4)
  */
#version 330 core

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
