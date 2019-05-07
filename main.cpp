/**
 * So this main file is a massive clusterfuck and a half -- I wanted this to be
 * more object oriented, but honestly I'm a tad pressed for time. So here we
 * are. I plan on continuing working with this over the summer, at which point
 * hopefully I will have something half-way organized.
 */
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <iostream>

#include "model.hpp"

#define bool_to_string(x)       (x ? "true" : "false")

char ebuf[2048];

double prev_x;
double prev_y;
double xoffset;
double yoffset;
float yaw = 0.0f;
float pitch = 0.0f;

void glfwErrorCb(int e, const char* msg)
{
        std::cout << "glfw:" << msg << std::endl;
}

void glfwMouseCb(GLFWwindow* wnd, double x, double y)
{
        xoffset = (x - prev_x) * 0.05;
        yoffset = (prev_y - y) * 0.05;

        prev_x = x;
        prev_y = y;

        yaw += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;
}

void GLAPIENTRY glErrorCb(GLenum src, GLenum type, GLenum id, 
                                GLenum sev, GLsizei len,
                                const GLchar* msg, const void* up)
{
        const char* ew = (sev == GL_DEBUG_TYPE_ERROR) ? "error: " : "warn: ";
        std::cout << ew << msg << std::endl;
}

int main(int argc, char* argv[])
{
        int rv;
        GLFWwindow* wnd;

        GLfloat background_color[] = {0.2f, 0.5f, 0.7f, 1.0f};
        GLfloat depth_clear[] = {1.0f};

        GLuint shader;

        glfwSetErrorCallback(glfwErrorCb);
        rv = glfwInit();
        if(rv != GLFW_TRUE) {

                exit(1);
        }

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        wnd = glfwCreateWindow(800, 600, "Title", nullptr, nullptr);
        if(!wnd) {

                exit(1);
        }

        glfwMakeContextCurrent(wnd);
        glfwSetCursorPosCallback(wnd, glfwMouseCb);
        glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        rv = glewInit();
        if(rv != GLEW_OK) {

                exit(1);
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);

        glDebugMessageCallback(glErrorCb, 0);

        /**
         * Load all the models we will be displaying
         * In the future, these models would best be placed into a graph, so
         * that we can just render everything within a for-loop.
         */
        Model floor(std::string("./resources/floorplan.obj"));
        Model brachi(std::string("./resources/brachi.obj"));
        Model para(std::string("./resources/para.obj"));
        Model rex(std::string("./resources/rex.obj"));

        shader = glCreateProgram();

        /**
         * We didn't have time to write a shader class -- so we have this here.
         */
        const char* vsource =   "# version 330 core\n"
                                "layout(location = 0) in vec3 position;\n"
                                "layout(location = 1) in vec3 normal;\n"
                                "layout(location = 2) in vec2 texCoord0;\n"
                                "out vec4 fragNormal;\n"
                                "out vec4 fragPosition;\n"
                                "out vec4 fragCoord;\n"
                                "uniform mat4 mvp;\n"
                                "void main() {\n"
                                "gl_Position = mvp * vec4(position, 1.0);\n"
                                "fragNormal = vec4(normal, 1.0);\n"
                                "fragPosition = vec4(position, 1.0);\n"
                                "fragCoord = vec4(texCoord0, 1.0, 1.0);\n"
                                "}\n";

        const char* fsource =   "# version 330 core\n"
                                "in vec4 fragNormal;\n"
                                "in vec4 fragPosition;\n"
                                "in vec4 fragCoord;\n"
                                "out vec4 outColor;\n"
                                "void main() {\n"
                                "float ambientStr = 0.1;\n"
                                "vec4 lightCol = vec4(0.5, 0.5, 1.0, 1.0);\n"
                                "vec4 lightPos = vec4(0.0);\n"
                                "vec4 lightDir = -vec4(-0.5, -0.5, -0.5, 0.0);\n"
                                "float dif = max(dot(fragNormal, lightDir), 0.0);"
                                "vec4 diffuse = dif * lightCol;"
                                "outColor = (ambientStr * lightCol + diffuse) * fragNormal;\n"
                                "}\n";

        GLuint vsh = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vsh, 1, &vsource, nullptr);
        glCompileShader(vsh);
        glGetShaderiv(vsh, GL_COMPILE_STATUS, &rv);
        if(rv != GL_TRUE) {
                glGetShaderInfoLog(vsh, 2048, nullptr, ebuf);
                std::cerr << "shader: " << ebuf << std::endl;
        }

        glAttachShader(shader, vsh);
        glDeleteShader(vsh);

        GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(fsh, 1, &fsource, nullptr);
        glCompileShader(fsh);
        glGetShaderiv(fsh, GL_COMPILE_STATUS, &rv);
        if(rv != GL_TRUE) {
                glGetShaderInfoLog(fsh, 2048, nullptr, ebuf);
                std::cerr << "shader: " << ebuf << std::endl;
        }

        glAttachShader(shader, fsh);
        glDeleteShader(fsh);

        glLinkProgram(shader);
        glGetProgramiv(shader, GL_LINK_STATUS, &rv);
        if(rv != GL_TRUE) { 
                glGetProgramInfoLog(shader, 2048, nullptr, ebuf);
                std::cerr << "shader: " << ebuf << std::endl;
        }

        glm::mat4 proj = glm::perspective(glm::radians(45.0f),
                                        800.0f / 600.0f,
                                        0.1f, 100.0f);

        /**
         * Set up all the model matrices for the meshes that we loadeded
         */
        glm::mat4 brachimodel = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, -2.0f)) *
                        glm::rotate(glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));


        glm::mat4 paramodel = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 3.0f)) *
                        glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(0.4f, 0.4f, 0.4f));

        glm::mat4 rexmodel = glm::translate(glm::mat4(1.0f), glm::vec3(6.0f, 0.0f, 2.0f)) *
                        glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

        glm::mat4 floormodel = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

        /**
         * For the first person camera
         */
        glm::vec3 playerPos = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 playerUp = glm::vec3(0.0f, 1.0f, 0.0f);

        while(!glfwWindowShouldClose(wnd)) {

                GLuint mvpLoc;
                glm::mat4 mvp;

                glClearBufferfv(GL_COLOR, 0, background_color);
                glClearBufferfv(GL_DEPTH, 0, depth_clear);

                glm::vec3 playerFront;

                playerFront.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
                playerFront.y = sin(glm::radians(pitch));
                playerFront.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
                playerFront = glm::normalize(playerFront);

                if(glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS) {

                        playerPos += 0.05f * playerFront;

                } if(glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS) {

                        playerPos -= 0.05f * playerFront;

                } if(glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS) {

                        playerPos -= 0.05f * glm::normalize(glm::cross(playerFront, playerUp));

                } if(glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS) {

                        playerPos += 0.05f * glm::normalize(glm::cross(playerFront, playerUp));
                }



                /**
                 * Camera stuff
                 */
                glm::mat4 view = glm::lookAt(playerPos,
                                                playerPos + playerFront,
                                                playerUp);

                mvp = proj * view * floormodel;

                glUseProgram(shader);
                mvpLoc = glGetUniformLocation(shader, "mvp");
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
                glUseProgram(0);

                floor.draw(shader);

                mvp = proj * view * brachimodel;

                glUseProgram(shader);
                mvpLoc = glGetUniformLocation(shader, "mvp");
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
                glUseProgram(0);

                brachi.draw(shader);

                mvp = proj * view * paramodel;

                glUseProgram(shader);
                mvpLoc = glGetUniformLocation(shader, "mvp");
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
                glUseProgram(0);

                para.draw(shader);

                mvp = proj * view * rexmodel;

                glUseProgram(shader);
                mvpLoc = glGetUniformLocation(shader, "mvp");
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
                glUseProgram(0);

                rex.draw(shader);

                glfwPollEvents();
                glfwSwapBuffers(wnd);
        }

        glfwTerminate();
}

