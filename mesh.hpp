#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {

        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord0;
};

class Mesh {

private:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        GLuint vao;

public:
        Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices);

        void draw(GLuint shaderProgram);
};


