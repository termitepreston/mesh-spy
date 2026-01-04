// lighting.frag
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gPBR;
uniform sampler2D environmentMap;

uniform vec3 viewPos;

const float PI = 3.14159265359;
const vec2 invAtan = vec2(0.1591, 0.3183);

// --- ACES Tone Mapping ---
vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// --- IBL Helper ---
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

// --- PBR Functions ---
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // 1. Retrieve data
    vec4 PosDepth = texture(gPosition, TexCoords);
    vec3 WorldPos = PosDepth.rgb;

    // Check if we hit background (Depth = 0 in GBuffer alpha usually implies empty if cleared)
    // However, we handle skybox separately. If normal is 0, discard?
    // Let's rely on standard depth test or masking.

    vec4 NormEmit = texture(gNormal, TexCoords);
    vec3 N = NormEmit.rgb;
    // Unpack normal (if 0,0,0 it's background)
    if (length(N) < 0.1) discard; // Let the skybox show through (rendered separately)

    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;

    vec3 PBR = texture(gPBR, TexCoords).rgb;
    float Metallic = PBR.r;
    float Roughness = PBR.g;
    float AO = PBR.b;

    vec3 V = normalize(viewPos - WorldPos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, Metallic);

    // 2. IBL Calculations

    // Ambient / Irradiance (Diffuse)
    // Approximation: Sample highly mip-mapped level of env map for irradiance
    // In a real engine, we'd use a convoluted irradiance map.
    vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, Roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - Metallic;

    // Sample high mip for diffuse (very blurry)
    vec2 uvDiffuse = SampleSphericalMap(N);
    vec3 irradiance = textureLod(environmentMap, uvDiffuse, 6.0).rgb; // Level 6 approx
    vec3 diffuse = irradiance * Albedo;

    // Specular IBL
    // Approximation: Sample based on roughness
    const float MAX_REFLECTION_LOD = 8.0; // Depends on texture mips
    vec2 uvSpec = SampleSphericalMap(R);
    vec3 prefilteredColor = textureLod(environmentMap, uvSpec, Roughness * MAX_REFLECTION_LOD).rgb;

    // Split-Sum approx (simplified integration)
    vec2 brdf  = vec2(0.9, 0.1); // Placeholder for LUT lookup
    vec3 specular = prefilteredColor * (F0 * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * AO;

    // 3. Output & Tone Mapping
    vec3 color = ambient;

    // ACES Tone Mapping
    color = aces(color);

    // Gamma Correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
