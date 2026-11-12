#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

layout(set = 2, binding = 0) uniform sampler2D shadowMap;

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
// Change the return type to vec3
vec3 calcShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||
       projCoords.y < 0.0 || projCoords.y > 1.0 ||
       projCoords.z < 0.0 || projCoords.z > 1.0) {
        return vec3(0.0, 1.0, 1.0); 
    }

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Better bias: larger on slopes, tiny on flat surfaces facing the light
    vec3 lightDir = normalize(-ubo.dirLights[0].direction.xyz);
    float cosTheta = clamp(dot(fragNormal, lightDir), 0.0, 1.0);
    float bias = max(0.005 * (1.0 - cosTheta), 0.0001); 

    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float depth = texture(shadowMap, projCoords.xy + vec2(float(x), float(y)) * texelSize).r;
            shadow += (currentDepth - bias > depth) ? 1.0 : 0.0;
        }
    }

    return vec3(shadow / 9.0, closestDepth, currentDepth);
}

#endif