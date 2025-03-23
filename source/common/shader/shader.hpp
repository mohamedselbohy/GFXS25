#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our
{

    class ShaderProgram
    {

    private:
        // Shader Program Handle (OpenGL object name)
        GLuint program;

    public:
        ShaderProgram()
        {
            // TODO: (Req 1) Create A shader program
            this->program = glCreateProgram();
        }
        ~ShaderProgram()
        {
            // TODO: (Req 1) Delete a shader program
            glDeleteProgram(this->program);
        }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use()
        {
            glUseProgram(program);
        }

        GLuint getUniformLocation(const std::string &name)
        {
            // TODO: (Req 1) Return the location of the uniform with the given name
            return glGetUniformLocation(this->program, name.c_str());
        }

        void set(const std::string &uniform, GLfloat value)
        {
            // TODO: (Req 1) Send the given float value to the given uniform
            glUniform1f(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, GLuint value)
        {
            // TODO: (Req 1) Send the given unsigned integer value to the given uniform
            glUniform1ui(this->getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, GLint value)
        {
            // TODO: (Req 1) Send the given integer value to the given uniform
            glUniform1i(this->getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, glm::vec2 value)
        {
            // TODO: (Req 1) Send the given 2D vector value to the given uniform
            glUniform2fv(this->getUniformLocation(uniform), 1, &value[0]);
        }

        void set(const std::string &uniform, glm::vec3 value)
        {
            // TODO: (Req 1) Send the given 3D vector value to the given uniform
            glUniform3fv(this->getUniformLocation(uniform), 1, &value[0]);
        }

        void set(const std::string &uniform, glm::vec4 value)
        {
            // TODO: (Req 1) Send the given 4D vector value to the given uniform
            glUniform4fv(this->getUniformLocation(uniform), 1, &value[0]);
        }

        void set(const std::string &uniform, glm::mat4 matrix)
        {
            // TODO: (Req 1) Send the given matrix 4x4 value to the given uniform
            glUniformMatrix4fv(this->getUniformLocation(uniform), 1, false, &matrix[0][0]);
        }

        // TODO: (Req 1) Delete the copy constructor and assignment operator.
        // Question: Why do we delete the copy constructor and assignment operator?
        // We delete copy constructors and assignment operators to abide with RAII rules
        // of only having one instance that points to our program
        // avoiding shallow copies or ownership issues
        ShaderProgram(const ShaderProgram &) = delete;            // To delete copy constructor
        ShaderProgram &operator=(const ShaderProgram &) = delete; // To delete assignment operator
    };

}

#endif