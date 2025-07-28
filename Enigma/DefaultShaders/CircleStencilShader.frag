#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform int StencilID;
const float Epsilon = 0.01;

void main() {

    vec2 point = TexCoord * 2 - 1;
    float dist = 1 - length(point);
    float stencil = step(0.0, dist);
    if(stencil < Epsilon) discard;

    int rslts = int(floor(stencil * 0xFF));
    int rslt = (rslts & 0xFF) | ((StencilID << 8) & 0xFFFFFF);

    FragColor = vec4(rslt);
}