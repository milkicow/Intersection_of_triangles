#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, 0.0, 0.0));
const float AMBIENT = 0.5;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    vec3 NormalWorldSpace = normalize(mat3(ubo.model) * inNormal);

    float LightIntensity = AMBIENT + max(dot(NormalWorldSpace, DIRECTION_TO_LIGHT), 0);

    fragColor = LightIntensity * inColor;
} 