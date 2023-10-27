#version 450

layout(location = 0) in vec3 fragPosition; // Interpolated vertex position
layout(location = 1) in vec3 fragNormal;   // Interpolated vertex normal
layout(location = 2) in vec2 fragTexCoord; // Interpolated texture coordinates

layout(location = 3) in vec4 inColor;      // Interpolated color from the vertex shader

layout(location = 0) out vec4 fragColor;   // Output color

layout(set = 0, binding = 0) buffer LightBuffer {
    vec3 lightDirection; // Direction of the light source
    vec3 viewPosition;   // Position of the viewer (camera)
};

layout(set = 0, binding = 1) uniform sampler2D textureSampler; // Texture sampler

const float shininess = 32.0; // Adjust the shininess parameter as needed

void main()
{
    // Calculate normalized light direction and normalized view direction
    vec3 L = normalize(lightDirection - fragPosition);
    vec3 V = normalize(viewPosition - fragPosition);

    // Calculate half-vector (Blinn-Phong halfway vector)
    vec3 H = normalize(L + V);

    // Calculate the diffuse and specular components
    float diff = max(dot(fragNormal, L), 0.0);
    float spec = pow(max(dot(fragNormal, H), 0.0), shininess);

    // Apply a texture using the sampler
    vec4 textureColor = texture(textureSampler, fragTexCoord);

    // Combine lighting and texture to determine the final color
    vec4 finalColor = inColor * textureColor * (diff + spec);

    // Output the final color
    fragColor = finalColor;
}
