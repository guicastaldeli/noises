#version 450

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
    vec4 cameraPos;

    vec4 ambientColor;
    int pointLightCount;
    int dirLightCount;
    vec2 padding;

    struct PointLight { vec4 position; vec4 color; } pointLights [32];
    struct DirLight { vec4 direction; vec4 color; } dirLights[4];

    mat4 lightSpaceMatrix;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 4) in mat4 instanceModel;

void main() {
    vec4 worldPos = instanceModel * vec4(inPosition, 1.0);
    gl_Position = ubo.lightSpaceMatrix * worldPos;
}