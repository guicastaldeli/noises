#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUv;
layout(location = 2) flat in uint fragTexIndex;

layout(set = 1, binding = 0) uniform sampler2D tex[];

layout(location = 0) out outColor;

void main() {
    vec4 val = texture(tex[nonuniformEXT(fragTexIndex)], fragUv);
    outColor = val * vec4(fragColor, 1.0);
}