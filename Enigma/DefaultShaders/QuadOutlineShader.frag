#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float Thickness;
uniform vec2 Bounds;

uniform int StencilID;
const float Epsilon = 0.01;

void main() {

    vec2 point = TexCoord * 2 - 1;
    point *= Bounds;
    float rThickness = Thickness * 0.25;

    float stencil = 0;
    if(point.x < -Bounds.x + rThickness || point.x > Bounds.x - rThickness) stencil = 1;
    else if(point.y < -Bounds.y + rThickness || point.y > Bounds.y - rThickness) stencil = 1;

    if(stencil < Epsilon) discard;

    int rslts = int(floor(stencil * 0xFF));
    int rslt = (rslts & 0xFF) | ((StencilID << 8) & 0xFFFFFF);

    FragColor = vec4(rslt);
}