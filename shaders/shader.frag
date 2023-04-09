#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} ubo;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

void main() {

    vec4 ambientLightColor = {1.0, 1.0, 1.0, 0.2};
    vec3 lightPosition = {1.0, 1.0, 1.0};
    vec4 lightColor = {1.0, 1.0, 1.0, 100.0};
    vec3 specularLight = vec3(0.0);

    vec3 directionToLight = lightPosition - fragPosWorld;
    float attenuation = 100.0 / dot(directionToLight, directionToLight);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 intensity = lightColor.xyz * lightColor.w * attenuation;
    vec3 ambientLight = ambientLightColor.xyz * ambientLightColor.w;
    vec3 diffuseLight = intensity * max(dot(surfaceNormal, normalize(directionToLight)), 0);

    vec3 viewDir = ubo.viewPos - fragPosWorld;
    vec3 halfAngle = normalize(directionToLight + viewDir);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 35.0);
    specularLight = intensity * blinnTerm;

    outColor = vec4((ambientLight + diffuseLight + specularLight) * fragColor, 1.0);
}
