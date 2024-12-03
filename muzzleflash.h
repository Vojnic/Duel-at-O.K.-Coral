#ifndef MUZZLEFLASH_H
#define MUZZLEFLASH_H

#include <GL/glew.h>
#include <string>

class MuzzleFlash {
public:
    MuzzleFlash(GLfloat* vertices, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    ~MuzzleFlash();

    void setShow(bool show);
    void render();

private:
    GLuint VAO, VBO, textureID, shaderProgram;
    bool showMuzzle;
    GLfloat* muzzleVertices;

    void loadTexture(const std::string& texturePath, GLuint& textureID);
    GLuint compileShaderFromFile(const std::string& path, GLenum type) const;
    void checkShaderCompilation(GLuint shader) const;
    void checkProgramLinking(GLuint program) const;
    std::string readShaderFile(const std::string& filePath) const;
};

#endif
