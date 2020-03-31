#version 410 core
precision mediump float;

uniform sampler2D tex;
uniform float alpha;
uniform mat4 invMatrix;
uniform vec3 lightDir;
uniform vec3 camPos;
uniform float noiseStrength;
uniform bool isNormal;

in vec4 vMirrorCoord;
in vec3 vNormal;
in float vNoise;

out vec4 fragColor;

void main() {
    vec3 invLight = normalize(invMatrix * vec4(lightDir, 0.0)).xyz;
    vec3 invEye = normalize(invMatrix * vec4(camPos, 0.0)).xyz;
    vec3 halfLE = normalize(invLight + invEye);
    float diffuse = clamp(dot(vNormal, invLight), 0.1, 1.0);
    float specular = pow(clamp(dot(vNormal, halfLE), 0.0, 1.0), 50.0);

    // vec3 projCoords = vMirrorCoord.xyz / vMirrorCoord.w;
    vec3 projCoords = vMirrorCoord.xyz;
    projCoords.z += vNoise * noiseStrength;
    if(isNormal) 
        fragColor = vec4(vNormal, 1.0);
    else 
        fragColor = vec4(textureProj(tex, projCoords.xyz).rgb * alpha + diffuse * 0.2 + specular, alpha);
}