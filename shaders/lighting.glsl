#ifndef LIGHITING_GLSL
#define LIGHTING_GLSL

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

        float att = 1.0 / (1.0 + 0.5 * dist + 0.2 * dist * dist);
        
        float diff = max(dot(fragNormal, normalize(toLight)), 0.0);

        vec3 lightCol = ubo.pointLights[i].color.rgb;
        float intensity = ubo.pointLights[i].color.a;

        ambientLightColor += color.rgb * diff * lightCol * intensity * att;
    }
}

// Calculate Shadow
float calcShadow(vec4 fragPosLightSpace) {

}