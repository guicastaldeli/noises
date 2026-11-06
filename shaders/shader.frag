#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUv;
layout(location = 2) flat in uint fragTexIndex;
layout(location = 3) flat in uint fragHasTexture;
layout(location = 4) in vec3 fragPosWorld;
layout(location = 5) in vec3 fragNormal;

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

    LightData pointLights[32];
    DirLightData dirLights[4];
} ubo;

layout(set = 1, binding = 0) uniform sampler2D tex[];

layout(location = 0) out vec4 outColor;

// Set Ambient Light
vec3 setAmbientLight(vec4 color) {
    vec3 c = color.rgb * ubo.ambientColor.rgb * ubo.ambientColor.a;
    return c;
}

// Set Directional Light
void setDirectionalLight(vec4 color, inout vec3 ambientLightColor) {
    for(int i = 0; i < ubo.dirLightCount; i++) {
        vec3 lightDir = normalize(-ubo.dirLights[i].direction.xyz);
        float diff = max(dot(fragNormal, lightDir), 0.0);

        vec3 lightCol = ubo.dirLights[i].color.rgb;
        float intensity = ubo.dirLights[i].color.a;

        ambientLightColor += color.rgb * diff * lightCol * intensity;
    }
}

// Set Point Light
void setPointLight(vec4 color, inout vec3 ambientLightColor) {
    for(int i = 0; i < ubo.pointLightCount; i++) {
        vec3 lightPos = ubo.pointLights[i].position.xyz;
        vec3 toLight = lightPos - fragPosWorld;
        float dist = length(toLight);
        vec3 lightDir = normalize(toLight);

        float att = max(1.0 - (dist / 10.0), 0.0);
        
        float diff = max(dot(fragNormal, normalize(toLight)), 0.0);

        vec3 lightCol = ubo.pointLights[i].color.rgb;
        float intensity = ubo.pointLights[i].color.a;

        ambientLightColor += color.rgb * diff * lightCol * intensity * att;
    }
}

/******
    Main
 */
void main() {
    vec4 baseColor = vec4(fragColor, 1.0);
    if(fragHasTexture != 0u) baseColor = texture(tex[nonuniformEXT(fragTexIndex)], fragUv);

    /* Ambient Light */ vec3 ambientLightColor = setAmbientLight(baseColor);
    /* Directional Light */ setDirectionalLight(baseColor, ambientLightColor);
    /* Point Light */ setPointLight(baseColor, ambientLightColor);

    outColor = vec4(ambientLightColor, 1.0);
}