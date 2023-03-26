#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, 0.0, 0.0));
const float AMBIENT = 0.5;

void main() {

    vec4 positionWorld = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

    fragNormalWorld= normalize(mat3(ubo.model) * inNormal);
    fragPosWorld = positionWorld.xyz;

    fragColor = inColor;
} 