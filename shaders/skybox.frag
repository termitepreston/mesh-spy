// skybox.frag
#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform sampler2D environmentMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
    vec3 color = texture(environmentMap, uv).rgb;

    // We do NOT tone map here if this is rendered into an FBO that supports HDR
    // (but our main framebuffer is usually default backbuffer).
    // However, in this deferred pipeline, Skybox is usually drawn LAST or
    // cleared first.
    // If we draw strictly into the GBuffer... wait, Skybox isn't in GBuffer.
    // Skybox is drawn to the default framebuffer (or composite pass) AFTER lighting.

    // For this specific architecture:
    // We will apply ACES here so it matches the Lighting pass output visually.

    color = color / (color + vec3(1.0)); // Simple Reinhard for safety
    // (Real ACES will be in the lighting pass, we'll keep skybox simple for now)

    FragColor = vec4(color, 1.0);
}
