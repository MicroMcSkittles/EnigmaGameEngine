#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 FragPos;

uniform sampler2D TextureMap;
uniform sampler2D StencilMap;
uniform int StencilID;
uniform vec4 Tint;
const float Epsilon = 1.0 / 255.0;

void main() {
    float stencil = 0;
    if(StencilID != -1) {
        int data = int(texture(StencilMap, FragPos).r);
        int s = data & 0xFF;
        int id = (data >> 8) & 0xFFFFFF;
        stencil = float(s) / 255.f;
        if(id != StencilID || stencil < Epsilon) discard;
    }
    else {
        stencil = 1;
    }

    vec4 textureColor = texture(TextureMap, TexCoord);
    if (textureColor.a < 0.5) discard;
    FragColor = textureColor * stencil * Tint;
}