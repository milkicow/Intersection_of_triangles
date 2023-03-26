#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

void main() {

    vec4 ambientLightColor = {1.0, 1.0, 1.0, 0.4};
    vec3 lightPosition = {1.0, 1.0, 1.0};
    vec4 lightColorU = {1.0, 1.0, 1.0, 1.0};

    vec3 directionToLight = lightPosition - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);

    vec3 lightColor = lightColorU.xyz * lightColorU.w * attenuation;
    vec3 ambientLight = ambientLightColor.xyz * ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

    outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}
