#version 410 core
#pragma include "noise.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

uniform float time;
uniform float noiseScale;
uniform float noiseStrength;
uniform mat4 mvpMatrix;
uniform mat4 transMatrix;
uniform mat4 mvMatrix;

struct Vertex { 
    vec3 position;
    vec3 normal;
    vec2 texcoord;
};

in Vertex vertex[];

out vec3 vNormal;
out vec4 vMirrorCoord;
out float vNoise;

Vertex calcNormal(float scale, int idx) {
    Vertex v0 = vertex[idx];
    Vertex v1 = vertex[(idx + 1) % 3];
    Vertex v2 = vertex[(idx + 2) % 3];
    vec2 deltaUV1 = v1.texcoord - v0.texcoord;
    vec2 deltaUV2 = v2.texcoord - v0.texcoord;
    vec3 deltaPos1 = v1.position - v0.position;
    vec3 deltaPos2 = v2.position - v0.position;

    float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
    vec3 binormal = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

    float delta = 0.05;

    vec3 vt = v0.position;
    vt += v0.normal * cnoise(vt * noiseScale + time) * noiseStrength;
    
    vec3 vt1 = vt + tangent * delta;
    vt1 += v0.normal * cnoise(vt1 * noiseScale + time) * noiseStrength;

    vec3 vt2 = vt + binormal * delta;
    vt2 += v0.normal * cnoise(vt2 * noiseScale + time) * noiseStrength;

    vec3 normal = normalize(cross(vt2 - vt, vt1 - vt));

    // with new normal and displaced position
    Vertex newV;
    newV.position = vt;
    newV.normal = normal;
    newV.texcoord = v0.texcoord;

    return newV;
}

void main() {
    for(int i=0; i<gl_in.length(); i++) {
        Vertex newV = calcNormal(10.0, i);
        gl_Position = mvpMatrix * vec4(newV.position, 1.0);
        vMirrorCoord = transMatrix * (mvMatrix * vec4(newV.position, 1.0));
        vNormal = newV.normal;
        vNoise = cnoise(vertex[i].position * noiseScale + time);
        EmitVertex();
    }
    EndPrimitive();
}