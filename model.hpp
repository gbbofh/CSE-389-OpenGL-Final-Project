#include "mesh.hpp"

#include <string>
#include <vector>
#include <GL/glew.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

class Model {

private:
        std::vector<Mesh> meshes;

        std::string path;
        std::string name;

        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);

public:
        Model(std::string path);

        void draw(GLuint shaderProgram);
};

