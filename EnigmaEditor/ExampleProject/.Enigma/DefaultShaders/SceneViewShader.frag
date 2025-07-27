#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D TextureMap;
uniform sampler2D StencilMap;
uniform vec4 Tint;
const float Epsilon = 0.001;

// bool fEqual(float v1, float v2) {
//     if(v1 < v2 + Epsilon && v1 > v2 - Epsilon) return true;
//     return false;
// }

void main() {
    float stencil = texture(StencilMap, TexCoord).r;
    //if (fEqual(stencil, 0)) discard;
    if(stencil < Epsilon) discard;
    vec4 textureColor = texture(TextureMap, TexCoord);
    if (textureColor.a < 0.5) discard;
    FragColor = textureColor * Tint;
}