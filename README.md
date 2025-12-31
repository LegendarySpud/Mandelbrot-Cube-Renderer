# Mandelbrot-Cube-Renderer
Renders a rotating cube with the mandelbrot set projected on its faces - uses OpenGL with C++

To run:
    Linux - Execute the executable
    Windows - Execute the .exe file
    Apple - Try compiling it

To compile:
    You can only compile while using Arch Linux
    
    Install dependencies - sudo pacman -S glfw-x11\
    . cd into mandelbrot/src
    Compile for Linux - make
    Compile for Windows - make exe
    Compile for apple - ¯\_(*.*)_/¯

How to use (made for version 3, some parts are applicable in versions 1 and 2):
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
        This program began as a simple 2d mandelbrot explorer (ver 1, 2) but became a rotating mandelbrot
        cube (ver 3+)
        Anything mentioning 64 bits is vestigial - from an attempt to implement a double precision
        rendering mode (failed) made before implementation of 3d.
