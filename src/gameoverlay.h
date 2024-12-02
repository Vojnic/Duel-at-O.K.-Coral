#pragma once
#include <GL/glew.h>
#include <string>

class GameOverlay {
public:
    GameOverlay(const std::string& retryTexturePath, const std::string& quitTexturePath);
    ~GameOverlay();

    void render() const;
    bool isRetryClicked(float mouseX, float mouseY) const;
    bool isQuitClicked(float mouseX, float mouseY) const;

private:
    GLuint VAO, VBO;
    GLuint retryTextureID, quitTextureID;
    GLuint shaderProgram;

    GLuint backgroundVAO, backgroundVBO;
    GLuint backgroundShaderProgram;

    void loadTexture(const std::string& texturePath, GLuint& textureID);
    GLuint compileShader(const char* source, GLenum type) const;
    void checkShaderCompilation(GLuint shader) const;
    void checkProgramLinking(GLuint program) const;

    // New helper for background rendering
    void setupBackground();
};
