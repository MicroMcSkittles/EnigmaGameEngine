#version 460 core
out vec4 FragColor;

const float Epsilon = 0.01;

in vec2 TexCoord;
in vec2 FragPos;

uniform sampler2D TextureMap;
uniform vec4 Tint;

void main() {

    vec2 point = TexCoord * 2 - 1;
    float dist = 1 - length(point);
    float stencil = step(0.0, dist);
    if(stencil < Epsilon) discard;

    vec4 textureColor = texture(TextureMap, TexCoord);
    if (textureColor.a < 0.5) discard;
    FragColor = textureColor * Tint;
}