#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

unsigned int Shader::compileShader(GLenum type, const char* source) {
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;

    if (file.is_open()) {
        ss << file.rdbuf();
        file.close();
    }
    else {
        std::cerr << "Error: Could not open file " << source << std::endl;
        return 0; // Return 0 for failure
    }

    std::string temp = ss.str();
    const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    int success;
    char infoLog[512];

    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader Compilation Error ("
            << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
            << "): " << infoLog << std::endl;
        return 0; // Return 0 for failure
    }

    return shader;
}

unsigned int Shader::createShader(const char* vsSource, const char* fsSource) {
    unsigned int program = glCreateProgram();
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    if (vertexShader == 0 || fragmentShader == 0) {
        return 0; // Return 0 for failure if either shader failed
    }

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Program Linking Error: " << infoLog << std::endl;
        return 0; // Return 0 for failure
    }

    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Program Validation Error: " << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
