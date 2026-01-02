#version 330 core
layout (location = 0) out vec4 gPosition; // RGB=WorldPos, A=Depth
layout (location = 1) out vec4 gNormal;   // RGB=Normal, A=Emissive
layout (location = 2) out vec4 gAlbedo;   // RGB=Albedo, A=Alpha
layout (location = 3) out vec4 gPBR;      // R=Metal, G=Rough, B=AO

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Material Uniforms (Placeholders until Phase 5)
uniform vec3 uAlbedoColor;
uniform float uMetallic;
uniform float uRoughness;

void main()
{
    // 1. Position + Depth
    gPosition.rgb = FragPos;
    gPosition.a = gl_FragCoord.z;

    // 2. Normal + Emissive (Placeholder 0.0)
    gNormal.rgb = normalize(Normal);
    gNormal.a = 0.0;

    // 3. Albedo + Alpha
    gAlbedo.rgb = uAlbedoColor;
    gAlbedo.a = 1.0;

    // 4. PBR: Metal, Rough, AO
    gPBR.r = uMetallic;
    gPBR.g = uRoughness;
    gPBR.b = 1.0; // AO placeholder
    gPBR.a = 0.0; // Unused
}
