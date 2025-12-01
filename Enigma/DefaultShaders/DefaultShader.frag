#version 450 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out int  EntityID;

//uniform int u_EntityID;
//uniform vec4 u_Tint;
//layout (binding = 0) uniform sampler2D u_TextureMap;

layout (std140, binding = 1) uniform Model {
    mat4 u_Model;
    vec4 u_Tint;
    int u_EntityID;
};

struct VertexOutput {
    vec2 texCoord;
};
layout (location = 0) in VertexOutput Input;

void main() {
    //vec4 textureColor = texture(u_TextureMap, Input.texCoord);
    //FragColor = textureColor * u_Tint;
    FragColor = u_Tint;
    EntityID = u_EntityID + 1;
}