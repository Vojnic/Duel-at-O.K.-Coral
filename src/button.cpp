#include "Button.h"
#include <iostream>
#include "stb_image.h"

// Constructor to initialize the button with texture and position/size
Button::Button(const std::string& texturePath, float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height) {

    GLfloat vertices[] = {
        // Positions        // Texture Coordinates
        x, y + height, 0.0f, 0.0f, 0.0f,  // Top-left
        x, y, 0.0f, 0.0f, 1.0f,            // Bottom-left
        x + width, y, 0.0f, 1.0f, 1.0f,    // Bottom-right
        x + width, y + height, 0.0f, 1.0f, 0.0f // Top-right
    };

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load the texture
    loadTexture(texturePath);

    // Shader sources
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        })";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D texture1;
        void main() {
            FragColor = texture(texture1, TexCoord);
        })";

    // Compile and link shaders
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinking(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Destructor to clean up OpenGL resources
Button::~Button() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &textureID);
    glDeleteProgram(shaderProgram);
}

// Renders the button
void Button::render() const {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Draw the button

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

// Checks if the button was clicked
bool Button::isClicked(double mouseX, double mouseY) const {
    // In OpenGL, the y-coordinate increases upwards, so we need to flip it for the mouse
    if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height) {
        return true; // The button was clicked
    }
    return false; // The button was not clicked
}

// Loads a texture from a file using stb_image
void Button::loadTexture(const std::string& texturePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
        return;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

// Compiles a shader from source
GLuint Button::compileShader(const char* source, GLenum type) const {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    checkShaderCompilation(shader);
    return shader;
}

// Checks for shader compilation errors
void Button::checkShaderCompilation(GLuint shader) const {
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Failed:\n" << infoLog << std::endl;
    }
}

// Checks for program linking errors
void Button::checkProgramLinking(GLuint program) const {
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program Linking Failed:\n" << infoLog << std::endl;
    }
}
