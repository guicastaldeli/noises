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

layout(location = 0) out vec3 fragColor;

void main() {
    vec4 worldPos = instanceModel * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;
    fragColor = inColor;
}