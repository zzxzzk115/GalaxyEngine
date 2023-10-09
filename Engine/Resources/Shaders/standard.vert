#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition; // Vertex position
layout(location = 1) in vec3 inNormal;   // Vertex normal
layout(location = 2) in vec2 inTexCoord; // Texture coordinates

out gl_PerVertex {
    vec4 gl_Position;
};

out vec3 fragPosition; // Position information needed for the fragment shader
out vec3 fragNormal;   // Normal information needed for the fragment shader
out vec2 fragTexCoord; // Texture coordinates needed for the fragment shader

layout(location = 0) out vec4 outColor; // Output color

uniform mat4 model;      // Model matrix
uniform mat4 view;       // View matrix
uniform mat4 projection; // Projection matrix

void main() 
{
    // Calculate vertex position and normal
    fragPosition = vec3(model * vec4(inPosition, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * inNormal;
    fragTexCoord = inTexCoord;

    // Transform vertex position to clip space
    gl_Position = projection * view * vec4(fragPosition, 1.0);

    // Pass color to the fragment shader (assuming your fragment shader requires color output)
    outColor = vec4(1.0, 1.0, 1.0, 1.0); // Here, white color is used as an example
}
