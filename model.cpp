#include "model.hpp"

#include <iostream>

Model::Model(std::string path)
{
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
                        aiProcess_Triangulate | aiProcess_FlipUVs);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
                        !scene->mRootNode) {
                return;
        }

        this->path = path.substr(0, path.find_last_of('/'));
        this->name = path.substr(path.find_last_of('/') + 1);

        processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
        for(int i = 0; i < node->mNumMeshes; i++) {
                aiMesh* m = scene->mMeshes[node->mMeshes[i]];
                this->meshes.push_back(processMesh(m, scene));
        }

        for(int i = 0; i < node->mNumChildren; i++) {

                processNode(node->mChildren[i], scene);
        }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
        std::vector<unsigned int> indices;
        std::vector<Vertex> vertices;

        std::cout << "Loading vertices..." << std::endl;

        for(int i = 0; i < mesh->mNumVertices; i++) {

                Vertex v;

                glm::vec3 position;
                glm::vec3 normal;
                glm::vec2 texCoord0;

                position.x = mesh->mVertices[i].x;
                position.y = mesh->mVertices[i].y;
                position.z = mesh->mVertices[i].z;

                normal.x = mesh->mNormals[i].x;
                normal.y = mesh->mNormals[i].y;
                normal.z = mesh->mNormals[i].z;

                if(mesh->mTextureCoords[0]) {

                        texCoord0.x = mesh->mTextureCoords[0][i].x;
                        texCoord0.y = mesh->mTextureCoords[0][i].y;

                } else {

                        texCoord0.x = 0.0f;
                        texCoord0.y = 0.0f;
                }

                v.position = position;
                v.normal = normal;
                v.texCoord0 = texCoord0;

                std::cout << "position = <";
                std::cout << v.position.x << ", ";
                std::cout << v.position.y << ", ";
                std::cout << v.position.z;
                std::cout << ">" << std::endl;

                std::cout << "normal = <";
                std::cout << v.normal.x << ", ";
                std::cout << v.normal.y << ", ";
                std::cout << v.normal.z;
                std::cout << ">" << std::endl;

                std::cout << "texCoord0 = <";
                std::cout << v.texCoord0.x << ", ";
                std::cout << v.texCoord0.y;
                std::cout << ">" << std::endl << std::endl;

                vertices.push_back(v);
        }

        std::cout << std::endl << "Loading faces..." << std::endl;

        for(int i = 0; i < mesh->mNumFaces; i++) {

                aiFace face = mesh->mFaces[i];

                std::cout << "face = <";

                for(int j = 0; j < face.mNumIndices; j++) {

                        std::cout << face.mIndices[j];

                        if(j < face.mNumIndices - 1) {

                                std::cout << ", ";

                        }

                        indices.push_back(face.mIndices[j]);
                }

                std::cout << ">" << std::endl;
        }

        return Mesh(vertices, indices);
}

void Model::draw(GLuint shaderProgram)
{
        for(int i = 0; i < this->meshes.size(); i++) {

                meshes[i].draw(shaderProgram);
        }
}

