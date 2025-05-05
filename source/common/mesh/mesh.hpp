#pragma once

#include <glad/gl.h>
#include <ostream>
#include <iostream>
#include "vertex.hpp"

namespace our
{

#define ATTRIB_LOC_POSITION 0
#define ATTRIB_LOC_COLOR 1
#define ATTRIB_LOC_TEXCOORD 2
#define ATTRIB_LOC_NORMAL 3

    class Mesh
    {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO = 0, EBO = 0;
        unsigned int VAO = 0;
        // We need to remember the number of elements that will be draw by glDrawElements
        GLsizei elementCount = 0;
        // Bounding box for the mesh
        glm::vec3 boundingBoxMin;
        glm::vec3 boundingBoxMax;

    public:
        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering
        void calculateBoundingBox(const std::vector<Vertex> &vertices)
        {
            if (vertices.empty())
                return; // Early exit if no vertices are present

            // Initialize bounding box to the first vertex
            boundingBoxMin = vertices[0].position;
            boundingBoxMax = vertices[0].position;

            // Loop through all vertices to find the min and max
            for (const auto &v : vertices)
            {
                boundingBoxMin = glm::min(boundingBoxMin, v.position);
                boundingBoxMax = glm::max(boundingBoxMax, v.position);
            }
        }
        glm::vec3 getBoundingBoxMin() const
        {
            return boundingBoxMin;
        }

        // Getter for bounding box max
        glm::vec3 getBoundingBoxMax() const
        {
            return boundingBoxMax;
        }
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &elements)
        {
            // TODO: (Req 2) Write this function
            //  remember to store the number of elements in "elementCount" since you will need it for drawing
            //  For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc

            this->elementCount = elements.size();

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // note that vertices.data() returns a pointer to the first element in the vector
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * sizeof(unsigned int), &elements[0], GL_STATIC_DRAW);

            // now we have binded the vertex buffer and vertex array and the element buffer
            // we want know to tell it how to read it

            glBindVertexArray(VAO);

            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)offsetof(Vertex, color));

            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tex_coord));

            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

            glBindVertexArray(0);
            calculateBoundingBox(vertices);
        }

        // this function should render the mesh
        void draw()
        {
            // TODO: (Req 2) Write this function
            if (VBO == 0 || EBO == 0)
            {
                std::cerr << "Error: VBO/EBO not initialized!" << std::endl;
                exit(1);
            }
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR)
            {
                std::cerr << "OpenGL Error in draw(): " << err << std::endl;
            }
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh()
        {
            // TODO: (Req 2) Write this function

            // unbind first - best practice
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            if (VBO)
                glDeleteBuffers(1, &VBO);
            if (EBO)
                glDeleteBuffers(1, &EBO);
            if (VAO)
                glDeleteVertexArrays(1, &VAO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}