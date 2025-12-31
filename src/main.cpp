#include <iostream>
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include "../include/shader.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/camera.h"

/*
How to use:
    Overview:
        Renders a rotating cube with the mandelbrot set projected on its faces with OpenGL
    Controls:
        Up/down arrow keys to increase/decrease iterations
        Space to print current position
        Page up to enable exploration mode
        Page down to disable it
        WASD keys to move in exploration mode
        Scroll wheel to zoom in/out in exploration mode

    Helpful variables:
        vec2 pos - position of camera
        double zoom - the zoom of the camera
        int scrX, scrY - window size
        int ssaa - level of supersampling anti-aliasing (each level increases frame buffer by n^2)
        int fbX, fbY - size of frame buffer
        double scrollVal - level of zoom
        double zoomVal - how many times zoom increases with every -1 decrease of scrollVal
        float t - time since start of program
        float dt - delta time per frame
        bool explorationMode - enables/disables exploration mode
        int maxIters - maximum iterations for the mandelbrot algorithm
        vec3 colour1, colour2 - The primary and secondary colours of the render
        int banding - Level of colour banding
        vector<Shot> shots - vector of camera shots that play when exploration mode is disabled

    Notes:
        This program began as a simple 2d mandelbrot explorer but became a rotating mandelbrot
        cube
        Anything mentioning 64 bits is vestigial - from an attempt to implement a double precision
        rendering mode (failed) before implementation of 3d.
        The colour of the final cube can be modified in fShader32.glsl by modifying the rgb values
        after "FragColour = "
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
glm::vec2 lerpVec2(glm::vec2 v1, glm::vec2 v2, float t);
float lerpFloat(float a, float b, float t);

struct Shot {
    glm::vec2 pos1;
    glm::vec2 pos2;
    float zoom1;
    float zoom2;
    float t;
};

float fPI = 3.141592653;
double dPI = 3.141592653;

int scrX = 1000;
int scrY = 1000;
int ssaa = 1;
int fbX = scrX * ssaa;
int fbY = scrY * ssaa;
int bits = 32;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

double scrollVal = 0.0;
double zoomVal = 1.25;
double zoom = 1.0f;
glm::dvec2 pos(-1.4013, 0.00041294);
float t;
float dt;
float shotTime = 0.0f;
bool explorationMode = false;

// Render settings
int maxIters = 1000;
glm::vec3 colour1(0.0f, 0.0f, 0.0f);
glm::vec3 colour2(0.0f, 1.0f, 1.0f);
int banding = 25;

float rect[] = {
    // Position        // UV
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 0.0f, 0.0f, 0.0f,

     1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,-1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, 1.0f, 0.0f, 1.0f, 1.0f
};

float cube[] = {
    // Position           // Texture   // Normal
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
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
    //glEnable(GL_DEPTH_TEST);

    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD initialisation failed.\n";
        return 0;
    }

    // settings
    glEnable(GL_DEPTH_TEST);

    // init viewport
    glViewport(0, 0, scrX, scrY);

    Shader shader32("shaders/p32/vShader32.glsl", "shaders/p32/fShader32.glsl");
    Shader shader64("shaders/p64/vShader64.glsl", "shaders/p64/fShader64.glsl");
    Shader screenShader("shaders/screen/vScreen.glsl", "shaders/screen/fScreen.glsl");

    // create frame buffer object
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // create colour texture
    GLuint colorTex;
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, fbX, fbY, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    GLuint depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT24,
        fbX,
        fbY
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        depthRBO
    );
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FBO not complete.\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // create rect vbo, vao
    unsigned int rectVBO, rectVAO;
    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // create cube vbo, vao
    unsigned int cubeVBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    std::vector<Shot> shots = {
        {{-1.4013f, 0.00041294f}, {-1.4013f, 0.00041294f}, -12.0f, -65.0f, 30.0f},
        {{-1.35653, 0.0685965}, {-1.36048, 0.0710716}, -29.0f, -29.0f, 40.0f}
    };

    float prevTime = 0.0f;
    int shotIndex = 0;

    while(!glfwWindowShouldClose(window)) {
        t = (float)glfwGetTime();
        dt = t - prevTime;
        prevTime = t;
        float fps = 1.0/dt;
        processInput(window);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, fbX, fbY);

        //glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClear(GL_COLOR_BUFFER_BIT);
        std::string title = "Mandelbrot - " +
                            std::to_string((int)fps) + "fps  " +
                            std::to_string(int(1.0/zoom)) + "x zoom  " +
                            //std::to_string(int(scrollVal)) + " zoom  " +
                            std::to_string(maxIters) + " iters";
        glfwSetWindowTitle(window, title.c_str());

        if (!explorationMode) {
            Shot s = shots[shotIndex];
            float shotProg = (t-shotTime) / s.t; // 0.0f - 1.0f
            if (shotProg > 1.0f) {
                shotIndex++;
                shotTime = t;
                if (shotIndex > shots.size()) {
                    shotIndex = 0;
                }
            }
        pos = lerpVec2(s.pos1, s.pos2, shotProg);
        scrollVal = lerpFloat(s.zoom1, s.zoom2, shotProg);
        //std::cout << scrollVal << "\n";
        }
        zoom = pow(zoomVal, scrollVal);

        float angle = 0.0f;
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(camera.Pos, camera.Pos + camera.Front, camera.Up);
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.FOV), (float)scrX/(float)scrY, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.0f));
        if (!explorationMode) angle = glm::radians(17.4538f*t);
        model = glm::rotate(model, angle, glm::vec3(0.9f, 0.6f, 0.1f));
        glm::mat4 matrix = projection * view * model;

        glm::mat4 effect = glm::mat4(1.0f);
        if (!explorationMode) angle = glm::radians(10.123f*t);
        effect = glm::rotate(effect, angle, glm::vec3(1.0f, 0.3f, 0.5f));

        if (bits == 32) {
            glEnable(GL_DEPTH_TEST);
            shader32.use();
            shader32.setFloat("zoom", (float)zoom);
            shader32.setVec2("pos", glm::vec2(pos));
            shader32.setInt("maxIters", maxIters);
            shader32.setMat4("mat", matrix);
            shader32.setMat4("effectMat", effect);
            shader32.setVec3("c1", colour1);
            shader32.setVec3("c2", colour2);
            shader32.setInt("banding", banding);
            //glBindTexture(GL_TEXTURE_2D, colorTex);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, scrX, scrY);
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);
            screenShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, colorTex);
            screenShader.setInt("screenTex", 0);
            glBindVertexArray(rectVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        else if (bits == 64) {
            shader64.use();
            shader64.setDouble("ratio", double(scrX)/double(scrY));
            shader64.setFloat("zoom", zoom);
            shader64.setVec2("pos", pos);
            shader64.setInt("maxIters", maxIters);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        }
    glDeleteVertexArrays(1, &rectVAO);
    glDeleteBuffers(1, &rectVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
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
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        shotTime = t;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        explorationMode = true;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        explorationMode = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollVal -= yoffset;
}


glm::vec2 lerpVec2(glm::vec2 v1, glm::vec2 v2, float t) {
    glm::vec2 r;

    r.x = v1.x + t*(v2.x-v1.x);
    r.y = v1.y + t*(v2.y-v1.y);

    return r;
}

float lerpFloat(float a, float b, float t) {
    float r;

    r = a + t*(b-a);

    return r;
}
