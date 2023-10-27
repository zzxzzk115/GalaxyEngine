#version 450

layout(location = 0) in vec3 inPosition; // Vertex position
layout(location = 1) in vec3 inNormal;   // Vertex normal
layout(location = 2) in vec2 inTexCoord; // Texture coordinates

layout(location = 0) out vec3 fragPosition; // Vertex position passed to the fragment shader
layout(location = 1) out vec3 fragNormal;   // Vertex normal passed to the fragment shader
layout(location = 2) out vec2 fragTexCoord; // Texture coordinates passed to the fragment shader

layout(location = 3) out vec4 outColor; // Output color

layout(set = 0, binding = 0) buffer Matrices {
    mat4 model;
    mat4 view;
    mat4 projection;
};

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
