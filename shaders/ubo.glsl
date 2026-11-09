#ifndef UBO_GLSL
#define UBO_GLSL

struct LightData {
    vec4 position;
    vec4 color;
};

struct DirLightData {
    vec4 direction;
    vec4 color;
};

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
    vec4 cameraPos;

    vec4 ambientColor;
    int pointLightCount;
    int dirLightCount;
    vec2 padding;

    LightData pointLights [32];
    DirLightData dirLights[4];

    mat4 lightSpaceMatrix;
    int shadowMapWidth;
    int shadowMapHeight;
    vec2 shadowPadding;
} ubo;

#endif