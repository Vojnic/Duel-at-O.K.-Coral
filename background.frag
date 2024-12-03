#version 330 core
out vec4 FragColor;    // Output color of the fragment
in vec2 TexCoord;      // Texture coordinates from the vertex shader

uniform sampler2D texture1;  // Texture sampler

void main() {
    FragColor = texture(texture1, TexCoord);  // Sample the texture at the given coordinates
}
