#version 410 core
#pragma include "noise.glsl"

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 transMatrix;
uniform float time;
uniform float tex_offset;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
};

in vec4 position;
in vec2 texcoord;
in vec3 normal;

out vec4 clipSpace;
out vec2 vTexCoord;
out vec3 vNormal;
out vec4 vMirrorCoord;
out float vNoise;
out Vertex vertex;

vec3 calcNormal(vec3 p, vec3 n, float scale) {
    float eps = 0.05;
    vec2 uv1 = p.xy + vec2(eps, 0.0);
    vec2 uv2 = p.xy + vec2(0.0, eps);
    vec2 deltaUV1 = uv1 - p.xy;
    vec2 deltaUV2 = uv2 - p.xy;
    vec3 deltaPos1 = vec3(deltaUV1, cnoise(uv1));
    vec3 deltaPos2 = vec3(deltaUV2, cnoise(uv2)); 

    float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV2.y * deltaUV2.x);
    vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    vec3 binormal = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    vec3 p1 = p + tangent * eps;
    p1 += n * cnoise(p1.xy) * scale;

    vec3 p2 = p + binormal * eps;
    p2 += n * cnoise(p2.xy) * scale;

    vec3 newNormal = normalize(cross(p2 - p, p1 - p));
    
    return newNormal;
}

void main() {
    vTexCoord = texcoord;

    float scale = 10.0;

    // classic perlin noise value
    float noise_val = cnoise(texcoord * tex_offset + time * 0.3);
    vNoise = noise_val;
    // new vertex position after displaced by noise 
    vec3 newVertex = position.xyz + noise_val * scale;
    // dynamically calculate normal
    vNormal = calcNormal(newVertex, normal, scale);
    // coordinate for mirror texture
    vMirrorCoord = transMatrix * (mvMatrix * vec4(newVertex, 1.0));
    gl_Position = mvpMatrix * vec4(newVertex, 1.0);
}
