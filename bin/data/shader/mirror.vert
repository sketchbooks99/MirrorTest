#version 410 core

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texcoord;
};

in vec4 position;
in vec2 texcoord;
in vec3 normal;

out Vertex vertex;

void main() {
    vertex.position = position.xyz;
    vertex.normal = normal;
    vertex.texcoord = texcoord;
    gl_Position = position;
}
