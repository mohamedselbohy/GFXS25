#include "shader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

// Forward definition for error checking functions
std::string checkForShaderCompilationErrors(GLuint shader);
std::string checkForLinkingErrors(GLuint program);

bool our::ShaderProgram::attach(const std::string &filename, GLenum type) const
{
    // Here, we open the file and read a string from it containing the GLSL code of our shader
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "ERROR: Couldn't open shader file: " << filename << std::endl;
        return false;
    }
    std::string sourceString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char *sourceCStr = sourceString.c_str();
    file.close();
    // TODO: Complete this function
    // Note: The function "checkForShaderCompilationErrors" checks if there is
    //  an error in the given shader. You should use it to check if there is a
    //  compilation error and print it so that you can know what is wrong with
    //  the shader. The returned string will be empty if there is no errors.
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCStr, nullptr); // Outsource shader content from char array
    glCompileShader(shader); // Compiles shader
    std::string errMsg = checkForShaderCompilationErrors(shader); // Gets error message coming out of compilation
    if (!errMsg.empty())
    {   // Printing error message
        std::cerr   << "Failed to load shader: " << filename << " \n"
                    << errMsg << std::endl;
        return false;
    }
    // We return true if the compilation succeeded
    glAttachShader(this->program, shader);
    glDeleteShader(shader);
    return true;
}

bool our::ShaderProgram::link() const
{
    // TODO: Complete this function
    // Note: The function "checkForLinkingErrors" checks if there is
    //  an error in the given program. You should use it to check if there is a
    //  linking error and print it so that you can know what is wrong with the
    //  program. The returned string will be empty if there is no errors.
    glLinkProgram(this->program);
    std::string errMsg = checkForLinkingErrors(this->program);
    if(errMsg.size() != 0){
        std::cerr   <<"Failed to link program\n"
                    << errMsg << std::endl;
        return false;
    }

     // Double-check linking status
    GLint linked;
    glGetProgramiv(this->program, GL_LINK_STATUS, &linked);
    if (!linked) {
        std::cerr << "Shader program linking failed unexpectedly!\n";
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////
// Function to check for compilation and linking error in shaders //
////////////////////////////////////////////////////////////////////

std::string checkForShaderCompilationErrors(GLuint shader)
{
    // Check and return any error in the compilation process
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);
        std::string errorLog(logStr);
        delete[] logStr;
        return errorLog;
    }
    return std::string();
}

std::string checkForLinkingErrors(GLuint program)
{
    // Check and return any error in the linking process
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status)
    {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetProgramInfoLog(program, length, nullptr, logStr);
        std::string error(logStr);
        delete[] logStr;
        return error;
    }
    return std::string();
}