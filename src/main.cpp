#include <iostream>
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include "../include/shader.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

float fPI = 3.141592653;
double dPI = 3.141592653;

int scrX = 1000;
int scrY = 1000;
int bits = 64;

double zoom = 1.0;
double zoomVal = 1.25;
double scrollVal = 0.0;
double scrollTarg = scrollVal;
int maxIters = 100;
glm::dvec2 pos(-0.5, 0.0);
float dt;

float rect[] = {
    -1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f, -1.0f,-1.0f, 0.0f,
     1.0f,-1.0f, 0.0f, -1.0f,-1.0f, 0.0f,  1.0f, 1.0f, 0.0f
};

int main() {
    std::cout << "Mandelbrot Test\n";

    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // init window
    GLFWwindow* window = glfwCreateWindow(scrX, scrY, "Mandelbrot", NULL, NULL);
    if (window == NULL) {
        std::cout << "GLFW window creation failed.\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD initialisation failed.\n";
        return 0;
    }

    // init viewport
    glViewport(0, 0, scrX, scrY);

    Shader shader32("shaders/p32/vShader32.glsl", "shaders/p32/fShader32.glsl");
    Shader shader64("shaders/p64/vShader64.glsl", "shaders/p64/fShader64.glsl");

    // create vbo, vao
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    float prevTime = 0.0f;

    while(!glfwWindowShouldClose(window)) {
        float time = (float)glfwGetTime();
        dt = time - prevTime;
        prevTime = time;
        float fps = 1.0/dt;
        std::string title = "Mandelbrot - " +
                            std::to_string((int)fps) + "fps  " +
                            std::to_string(int(1.0/zoom)) + "x zoom  " +
                            std::to_string(maxIters) + " iters";
        glfwSetWindowTitle(window, title.c_str());
        processInput(window);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (bits = 32) {
            shader32.use();
            shader32.setFloat("ratio", float(scrX)/float(scrY));
            shader32.setFloat("zoom", (float)zoom);
            shader32.setVec2("pos", glm::vec2(pos));
            shader32.setInt("maxIters", maxIters);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        else if (bits = 32) {
            shader64.use();
            shader64.setDouble("ratio", double(scrX)/double(scrY));
            shader64.setFloat("zoom", zoom);
            shader64.setVec2("pos", pos);
            shader64.setInt("maxIters", maxIters);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    shader64.del();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    scrX = width;
    scrY = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    double dDt = double(dt);
    double z = zoom;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        maxIters += 1;
        if (maxIters < 0) {maxIters = 0;}
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        maxIters -= 1;
        if (maxIters < 0) {maxIters = 0;}
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        pos.y += z*dDt;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        pos.y -= z*dDt;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        pos.x += z*dDt;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        pos.x -= z*dDt;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        std::cout << "(" << pos.x << ", " << pos.y << ")\n";
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollTarg -= yoffset;
    scrollVal = scrollTarg;
    zoom = pow(zoomVal, scrollVal);
}
