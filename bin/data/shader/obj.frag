#version 410 core
precision mediump float;

uniform mat4 invMatrix;
uniform vec3 lightDir;
uniform vec3 camPos;

in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vColor;

out vec4 fragColor;

void main() {
    vec3 invLight = normalize(invMatrix * vec4(normalize(lightDir), 0.0)).xyz;
    vec3 invEye = normalize(invMatrix * vec4(camPos, 0.0)).xyz;
    vec3 halfLE = normalize(invLight + invEye);
    float diffuse = clamp(dot(vNormal, invLight), 0.1, 1.0);
    // float specular = pow(clamp(dot(vNormal, halfLE), 0.0, 1.0), 20.0);
    float specular = pow(diffuse, 20.0);
    fragColor = vec4(0.8, 0.8, 0.2, 1.0) * vec4(vec3(diffuse), 1.0) + vec4(vec3(specular), 1.0);
}