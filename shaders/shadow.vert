#version 450
#include "ubo.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 4) in mat4 instanceModel;

void main() {
    vec4 worldPos = instanceModel * vec4(inPosition, 1.0);
    gl_Position = ubo.lightSpaceMatrix * worldPos;
}