#include "muzzleflash.h"
#include <iostream>
#include "stb_image.h"
#include <fstream>
#include <sstream>

MuzzleFlash::MuzzleFlash(GLfloat* vertices, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    showMuzzle = false;  // By default, muzzle flash is not shown
    muzzleVertices = vertices;  // Store the vertices passed to the constructor

    // Load muzzle flash texture (ensure you have a muzzle flash image)
    loadTexture("Resources/muzzleflash.png", textureID);

    // Generate VAO and VBO for muzzle flash
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 5 * 4, muzzleVertices, GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load shader sources from file paths
    std::string vertexShaderSource = readShaderFile(vertexShaderPath);
    std::string fragmentShaderSource = readShaderFile(fragmentShaderPath);

    GLuint vertexShader = compileShaderFromFile(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShaderFromFile(fragmentShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    checkProgramLinking(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

MuzzleFlash::~MuzzleFlash() {
    glDeleteTextures(1, &textureID);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
}

void MuzzleFlash::loadTexture(const std::string& texturePath, GLuint& textureID) {
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

void MuzzleFlash::setShow(bool show) {
    showMuzzle = show;  // Set the flag to control whether the muzzle flash is visible
}

void MuzzleFlash::render() {
    if (!showMuzzle) {
        return;  // Don't render anything if muzzle flash is not active
    }

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);  // Draw the muzzle flash

    glBindVertexArray(0);
    glUseProgram(0);
}

GLuint MuzzleFlash::compileShaderFromFile(const std::string& source, GLenum type) const {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkShaderCompilation(shader);
    return shader;
}

void MuzzleFlash::checkShaderCompilation(GLuint shader) const {
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Failed:\n" << infoLog << std::endl;
    }
}

void MuzzleFlash::checkProgramLinking(GLuint program) const {
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program Linking Failed:\n" << infoLog << std::endl;
    }
}

std::string MuzzleFlash::readShaderFile(const std::string& filePath) const {
    std::ifstream shaderFile(filePath);
    std::stringstream shaderStream;
    if (shaderFile.is_open()) {
        shaderStream << shaderFile.rdbuf();
    }
    else {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
    }
    return shaderStream.str();
}
