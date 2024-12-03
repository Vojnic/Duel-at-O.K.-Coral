#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <string>
#include <GL/glew.h>
#include "Shader.h" // Include the Shader header

class Background {
public:
    Background(const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~Background();

    void render();

private:
    GLuint VAO, VBO;
    GLuint textureID;
    GLuint shaderProgram;

    void loadTexture(const std::string& texturePath);
};

#endif // BACKGROUND_H
