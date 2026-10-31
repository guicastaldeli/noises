#version 450

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUv;
layout(location = 3) in vec3 inColor;
layout(location = 4) in mat4 instanceModel;
layout(location = 8) in uint inTexIndex;
layout(location = 9) in uint inHasTexture;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUv;
layout(location = 2) flat out uint fragTexIndex;
layout(location = 3) flat out uint fragHasTexture;

void main() {
    vec4 terrainPos = instanceModel * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * terrainPos;
    
    fragColor = inColor;
    fragUv = inUv;
    fragTexIndex = inTexIndex;
    fragHasTexture = inHasTexture;
}