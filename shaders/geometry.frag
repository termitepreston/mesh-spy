// geometry.frag
#version 330 core
layout (location = 0) out vec4 gPosition; // RGB=WorldPos, A=Depth
layout (location = 1) out vec4 gNormal;   // RGB=Normal, A=Emissive
layout (location = 2) out vec4 gAlbedo;   // RGB=Albedo, A=Alpha
layout (location = 3) out vec4 gPBR;      // R=Metal, G=Rough, B=AO

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Material Factors
uniform vec4 uBaseColorFactor;
uniform float uMetallicFactor;
uniform float uRoughnessFactor;

// Toggles (From UI)
uniform bool uUseBaseColorMap;
uniform bool uUseMetallicMap;
uniform bool uUseRoughnessMap;
uniform bool uUseNormalMap;

// Texture Availability (From Model)
uniform bool uHasBaseColorMap;
uniform bool uHasMetallicRoughnessMap;
uniform bool uHasNormalMap;

// Samplers
uniform sampler2D texture_baseColor;
uniform sampler2D texture_metallicRoughness; // G=Roughness, B=Metal
uniform sampler2D texture_normal;

vec3 getNormalFromMap()
{
    // If map disabled or missing, return geometry normal
    if (!uUseNormalMap || !uHasNormalMap)
        return normalize(Normal);

    vec3 tangentNormal = texture(texture_normal, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    // 1. Position + Depth
    gPosition.rgb = FragPos;
    gPosition.a = gl_FragCoord.z;

    // 2. Normal
    gNormal.rgb = getNormalFromMap();
    gNormal.a = 0.0; // Emissive placeholder

    // 3. Albedo
    vec4 albedo = uBaseColorFactor;
    if (uUseBaseColorMap && uHasBaseColorMap) {
        // Sample texture and multiply by factor
        vec4 texColor = texture(texture_baseColor, TexCoords);
        // Assuming texture is sRGB, convert to Linear if needed,
        // but for now we treat as linear in deferred pass or handle in resolve.
        // GLTF defines baseColor as sRGB.
        // Simple approx gamma correction: pow(texColor.rgb, vec3(2.2))
        albedo *= texColor;
    }
    gAlbedo = albedo;

    // 4. PBR
    float metallic = uMetallicFactor;
    float roughness = uRoughnessFactor;

    if (uHasMetallicRoughnessMap) {
        vec4 mrSample = texture(texture_metallicRoughness, TexCoords);
        if (uUseRoughnessMap) roughness *= mrSample.g;
        if (uUseMetallicMap) metallic *= mrSample.b;
    }

    // Safety: Clamp roughness to avoid perfect mathematical singularities
    roughness = max(roughness, 0.04);

    gPBR.r = metallic;
    gPBR.g = roughness;

    // GLTF Metallic-Roughness packing: G = Roughness, B = Metallic
    if (uHasMetallicRoughnessMap) {
        vec4 mrSample = texture(texture_metallicRoughness, TexCoords);

        if (uUseRoughnessMap) {
            roughness *= mrSample.g;
        }
        if (uUseMetallicMap) {
            metallic *= mrSample.b;
        }
    }

    gPBR.r = metallic;
    gPBR.g = roughness;
    gPBR.b = 1.0; // AO placeholder
    gPBR.a = 0.0;
}
