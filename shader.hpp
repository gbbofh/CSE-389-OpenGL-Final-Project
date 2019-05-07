#include <string>
#include <vector>

class Shader {

public:
        void loadVertexShader(std::string path);
        void loadFragmentShader(std::string path);

        void compile();
        void use();

        void setUniform(int i);
        void setUniform(float f);

private:
        GLuint program;
        std::vector<GLuint> shaders;
};

