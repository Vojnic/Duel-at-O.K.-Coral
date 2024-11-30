#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <GL/glew.h>

class Player {
public:
    enum class State { Idle, Ready, Dead };

    Player(const std::string& idleTexturePath,
        const std::string& readyTexturePath,
        const std::string& deadTexturePath,
        GLfloat* initialVertices);  // Pass vertices for position
    ~Player();

    void setState(State newState);
    void addPoints();
	int getPoints();
    Player::State getState();
    void render();

private:
    void loadTexture(const std::string& texturePath, GLuint& textureID);

    GLuint VAO, VBO;
    GLuint textureIdleID, textureReadyID, textureDeadID;  // Separate texture IDs for each state
    GLuint shaderProgram;
    State currentState;
    int points;
    GLfloat* playerVertices;  // Pointer to vertices for position customization
};

#endif
