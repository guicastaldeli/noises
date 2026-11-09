#version 450
#extension GL_EXT_nonuniform_qualifier : require

#include "ubo.glsl"
#include "lighting.glsl"

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUv;
layout(location = 2) flat in uint fragTexIndex;
layout(location = 3) flat in uint fragHasTexture;
layout(location = 4) in vec3 fragPosWorld;
layout(location = 5) in vec3 fragNormal;
layout(location = 6) flat in uint fragHasLit;

layout(set = 1, binding = 0) uniform sampler2D tex[];

layout(location = 0) out vec4 outColor;

/******
    Main
 */
void main() {
    vec4 baseColor = vec4(fragColor, 1.0);
    if(fragHasTexture != 0u) baseColor = texture(tex[nonuniformEXT(fragTexIndex)], fragUv);
    vec3 viewDir = normalize(ubo.cameraPos.xyz - fragPosWorld);
    if(fragHasLit == 0u) {
        outColor = baseColor;
        return;
    }

    /* Ambient Light */ vec3 ambientLightColor = setAmbientLight(baseColor);
    /* Directional Light */ setDirectionalLight(baseColor, ambientLightColor);
    /* Point Light */ setPointLight(baseColor, ambientLightColor);

    vec4 fragPosLightSpace = ubo.lightSpaceMatrix * vec4(fragPosWorld, 0);

    float shadow = calcShadow(fragPosLightSpace);
    outColor = vec4(mix(ambientLightColor, setAmbientLight(baseCOlor), shadow), 1.0);
    return;
}