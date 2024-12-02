#ifndef MUZZLEFLASH_H
#define MUZZLEFLASH_H

#include <GL/glew.h>
#include <string>

class MuzzleFlash {
public:
    MuzzleFlash(GLfloat* vertices);
    ~MuzzleFlash();

    void setShow(bool show);
    void render();

private:
    GLuint VAO, VBO, textureID, shaderProgram;
    bool showMuzzle;
    GLfloat* muzzleVertices;
    void loadTexture(const std::string& texturePath, GLuint& textureID);
};

#endif
