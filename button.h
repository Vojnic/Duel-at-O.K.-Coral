#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <GL/glew.h>

class Button {
public:
    Button(const std::string& texturePath, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, float x, float y, float width, float height);

    ~Button();

    void render() const;

    bool isClicked(double mouseX, double mouseY) const;

private:
    GLuint VAO, VBO, textureID, shaderProgram;

    float x, y, width, height;

    void loadTexture(const std::string& texturePath);

    GLuint compileShaderFromFile(const std::string& filePath, GLenum type) const;

    void checkShaderCompilation(GLuint shader) const;
    void checkProgramLinking(GLuint program) const;

    std::string loadShaderSourceFromFile(const std::string& filePath) const;
};

#endif
