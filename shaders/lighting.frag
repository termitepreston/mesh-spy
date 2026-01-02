#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gPBR;

void main()
{
    // Retrieve data from G-buffer
    vec4 PosDepth = texture(gPosition, TexCoords);
    vec4 NormEmit = texture(gNormal, TexCoords);
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    vec3 PBR = texture(gPBR, TexCoords).rgb; // Metal, Rough, AO

    // DEBUG VISUALIZATION (Verify G-Buffer is working)
    // Uncomment one of these lines to debug specific channels

    // FragColor = vec4(Albedo, 1.0);
    // FragColor = vec4(NormEmit.rgb * 0.5 + 0.5, 1.0); // Visualizing Normals
    // FragColor = vec4(PBR, 1.0);

    // Simple placeholder lighting to prove geometry exists
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(NormEmit.rgb, lightDir), 0.0);
    vec3 diffuse = diff * Albedo;

    FragColor = vec4(diffuse, 1.0);
}
