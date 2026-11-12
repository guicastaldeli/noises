#version 450
#extension GL_EXT_nonuniform_qualifier : require

#include "ubo.glsl"

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUv;
layout(location = 2) flat in uint fragTexIndex;
layout(location = 3) flat in uint fragHasTexture;
layout(location = 4) in vec3 fragPosWorld;
layout(location = 5) in vec3 fragNormal;
layout(location = 6) flat in uint fragHasLit;

layout(set = 1, binding = 0) uniform sampler2D tex[];

#include "lighting.glsl"

layout(location = 0) out vec4 outColor;

void main() {
    // Get the base color (either the mesh's vertex color or the texture)
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

    vec4 fragPosLightSpace = ubo.lightSpaceMatrix * vec4(fragPosWorld, 1.0);
    
    // Get the raw shadow data
    vec3 shadowData = calcShadow(fragPosLightSpace);
    
    // --- DEBUG OUTPUT ---
    // shadowData = (shadow, closestDepth, currentDepth)
    // Multiply the base color by the raw debug data
    outColor = vec4(baseColor.rgb * shadowData, 1.0);
    
    return;
    // --------------------

    // Standard lighting code
    float shadow = shadowData.x;
    vec3 litColor = ambientLightColor;
    vec3 shadowColor = vec3(0.0, 0.0, 0.0);
    vec3 finalColor = mix(litColor, shadowColor, shadow);

    outColor = vec4(finalColor, 1.0);
}