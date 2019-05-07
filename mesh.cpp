
#include "mesh.hpp"


Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices)
{
        GLuint vbo;
        GLuint ebo;

        this->indices = std::vector<unsigned int>(indices);
        this->vertices = std::vector<Vertex>(vertices);

        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenVertexArrays(1, &this->vao);

        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
                        &this->vertices[0].position, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex), nullptr);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, texCoord0));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        this->indices.size() * sizeof(unsigned int),
                        &this->indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);

        //glDeleteBuffers(1, &vbo);
        //glDeleteBuffers(1, &ebo);
}

void Mesh::draw(GLuint shaderProgram)
{
        glUseProgram(shaderProgram);
        glBindVertexArray(this->vao);

        glDrawElements(GL_TRIANGLES, this->indices.size(),
                        GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glUseProgram(0);
}

