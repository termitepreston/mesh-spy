#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    WorldPos = aPos;
    // Remove translation from view matrix to keep skybox centered
    mat4 rotView = mat4(mat3(view));
    vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

    // Optimization: force depth to 1.0 (max depth)
    gl_Position = clipPos.xyww;
}
