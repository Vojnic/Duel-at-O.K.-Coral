#include "Player.h"
#include <iostream>
#include "stb_image.h"

// Constructor: Loads textures and initializes player with position
Player::Player(const std::string& idleTexturePath,
    const std::string& readyTexturePath,
    const std::string& deadTexturePath,
    GLfloat* initialVertices)  // Pass vertices as a parameter
{
    // Initialize player state to Idle
    currentState = State::Idle;
    playerVertices = initialVertices;  // Store the passed-in vertices

    // Load textures for all 3 states
    loadTexture(idleTexturePath, textureIdleID);  // Idle texture for player state
    loadTexture(readyTexturePath, textureReadyID);  // Ready texture
    loadTexture(deadTexturePath, textureDeadID);  // Dead texture

    // Generate VAO and VBO for the player
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 5 * 4, playerVertices, GL_STATIC_DRAW);  // Update buffer with passed vertices

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);  // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));  // Texture
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create shaders
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoord;
        uniform sampler2D texture1;
        void main() {
            FragColor = texture(texture1, TexCoord);
        }
    )";

    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Destructor
Player::~Player() {
    glDeleteTextures(1, &textureIdleID);
    glDeleteTextures(1, &textureReadyID);
    glDeleteTextures(1, &textureDeadID);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
}

// Load texture from file using stb_image
void Player::loadTexture(const std::string& texturePath, GLuint& textureID) {
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

    // Determine format
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

// Set the current state of the player and update the texture
void Player::setState(State newState) {
    currentState = newState;
}
Player::State Player::getState() {
    return currentState;
}

// Render the player with the correct texture based on the current state
void Player::render() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    GLuint currentTextureID = textureIdleID;  // Default to Idle texture

    // Choose the appropriate texture based on the current state
    switch (currentState) {
    case State::Ready:
        currentTextureID = textureReadyID;
        break;
    case State::Dead:
        currentTextureID = textureDeadID;
        break;
    case State::Idle:
    default:
        currentTextureID = textureIdleID;
        break;
    }

    // Bind the appropriate texture
    glBindTexture(GL_TEXTURE_2D, currentTextureID);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // Draw the player

    glBindVertexArray(0);
    glUseProgram(0);
}
