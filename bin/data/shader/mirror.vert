#version 410 core
#pragma include "noise.glsl"

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 transMatrix;
uniform float time;

in vec4 position;
in vec2 texcoord;
in vec3 normal;

out vec4 clipSpace;
out vec2 vTexCoord;
out vec3 vNormal;
out vec4 vMirrorCoord;
out float vNoise;

vec3 calcNormal(vec3 p) {
    float eps = 0.5;
    vec2 uv1 = p.xy + vec2(eps, 0.0);
    vec2 uv2 = p.xy + vec2(0.0, eps);

    vec3 v1 = vec3(uv1,cnoise(uv1));
    vec3 v2 = vec3(uv2,cnoise(uv2));
    v1 = normalize(v1 - p);
    v2 = normalize(v2 - p);

    vec3 n = normalize(cross(v1, v2));
    return n;
}

void main() {
    vTexCoord = texcoord;

    float scale = 10.0;

    // classic perlin noise value
    float noise_val = cnoise(texcoord * 10.0 + time * 0.3);
    vNoise = noise_val;
    // new vertex position after displaced by noise 
    vec3 newVertex = position.xyz + vec3(0.0, 0.0, noise_val * scale);
    // dynamically calculate normal
    vNormal = calcNormal(vec3(texcoord, noise_val));
    // coordinate for mirror texture
    vMirrorCoord = transMatrix * (mvMatrix * vec4(newVertex, 1.0));
    gl_Position = mvpMatrix * vec4(newVertex, 1.0);
}
