#version 410 core


uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform bool mirror;

in vec4 position;
in vec3 normal;
in vec2 texcoord;
in vec4 color;

out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vColor;

void main() {
    vNormal = normal;
    vTexCoord = texcoord;
    vColor = color;
    vec4 pos = mMatrix * position;
    if(mirror) pos = vec4(pos.x, -pos.y, pos.zw);
    gl_Position = vpMatrix * pos;
}