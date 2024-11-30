#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <GL/glew.h>

class Button {
public:
    Button(const std::string& texturePath, float x, float y, float width, float height);

    ~Button();

    void render() const;

    bool isClicked(double mouseX, double mouseY) const;

private:
    GLuint VAO, VBO, textureID, shaderProgram;

    float x, y, width, height;

    void loadTexture(const std::string& texturePath);

    GLuint compileShader(const char* source, GLenum type) const;

    void checkShaderCompilation(GLuint shader) const;
    void checkProgramLinking(GLuint program) const;
};

#endif
