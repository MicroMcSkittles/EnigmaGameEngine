#version 450 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out int  EntityID;

//layout (binding = 0) uniform sampler2D u_TextureMap;

layout (std140, binding = 1) uniform Model {
    mat4 u_Model;
    vec4 u_Tint;
    int u_EntityID;
    float u_Thickness;
    float u_Fade;
};

struct VertexOutput {
    vec2 texCoord;
};
layout (location = 0) in VertexOutput Input;

const float c_Epsilon = 0.000001f;
bool FloatEquals(float f1, float f2) {
    return (f1 <= f2 + c_Epsilon && f1 >= f2 - c_Epsilon);
}

void main() {
    vec2 localPosition = 2.0f * Input.texCoord - 1.0f;

    float dist = 1.0f - length(localPosition);
    float value = smoothstep(0.0, u_Fade, dist);
    value *= smoothstep(u_Thickness + u_Fade, u_Thickness, dist);

    //vec4 textureColor = texture(u_TextureMap, Input.texCoord);
    //FragColor = textureColor * u_Tint;
    FragColor = u_Tint * value;
    EntityID = u_EntityID + 1;
    if (FloatEquals(value, 0.0f)) discard;
}