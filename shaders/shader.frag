#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUv;
layout(location = 2) flat in uint fragTexIndex;
layout(location = 3) flat in uint fragHasTexture;

layout(set = 1, binding = 0) uniform sampler2D tex[];

layout(location = 0) out vec4 outColor;

void main() {
    if(fragHasTexture != 0u) {
        vec4 t = texture(tex[nonuniformEXT(fragTexIndex)], fragUv);
        outColor = vec4(t.rgb, 1.0);
    } else {
        outColor = vec4(fragColor, 1.0);
    }
}