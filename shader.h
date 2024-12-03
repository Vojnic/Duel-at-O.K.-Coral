#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h> // Make sure to include GLEW or appropriate OpenGL headers

class Shader {
public:
    Shader() = default;
    ~Shader() = default;

    // Compiles a single shader of a given type
    static unsigned int compileShader(GLenum type, const char* source);

    // Creates a shader program using vertex and fragment shader source files
    static unsigned int createShader(const char* vsSource, const char* fsSource);
};

#endif // SHADER_H