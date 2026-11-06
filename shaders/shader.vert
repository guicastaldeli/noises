#version 450

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUv;
layout(location = 3) in vec3 inColor;
layout(location = 4) in mat4 instanceModel;
layout(location = 8) in uint inTexIndex;
layout(location = 9) in uint inHasTexture;
layout(location = 10) in uint inHasLit;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUv;
layout(location = 2) flat out uint fragTexIndex;
layout(location = 3) flat out uint fragHasTexture;
layout(location = 4) out vec3 fragPosWorld;
layout(location = 5) out vec3 fragNormal;
layout(location = 6) flat out uint fragHasLit;

void main() {
    vec4 worldPos = instanceModel * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;
    
    fragColor = inColor;
    fragUv = inUv;
    fragTexIndex = inTexIndex;
    fragHasTexture = inHasTexture;
    fragHasLit = inHasLit;

    fragPosWorld = worldPos.xyz;
    fragNormal = normalize(mat3(instanceModel) * inNormal);
}