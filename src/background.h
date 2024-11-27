#ifndef background_h
#define background_h

#include <string>
#include <GL/glew.h>

class Background {
public:
    Background(const std::string& texturePath);
    ~Background();
    void render();

private:
    void loadTexture(const std::string& texturePath);

    GLuint VAO, VBO;
    GLuint textureID;
    GLuint shaderProgram;
};

#endif
