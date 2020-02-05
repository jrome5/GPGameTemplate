#pragma once

// Simplified Renderer application for GP course
// Features:
// Reduced OpenGL version from 4.5 to 3.3 to allow it to render in older laptops.
// Added Shapes library for rendering cubes, spheres and vectors.
// Added examples of matrix multiplication on Update.
// Added resize screen and keyboard callbacks.
// Added FPS camera functionality
// Update 2019/01 updated libraries and created project for VS2017 including directory dependant links to libraries.
// Update 2020/01 updated libraries for x64 and for VS2020, also adding MAC compiled Libraries.

// Suggestions or extra help please do email me at S.Padilla@hw.ac.uk

// Standard C++ libraries
#include <iostream>
#include <vector>
using namespace std;

// Helper graphic libraries
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "graphics.h"
#include "shapes.h"

// MAIN FUNCTIONS
void startup();
void updateCamera();
void updateSceneElements();
void renderScene();

// CALLBACK FUNCTIONS
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);

// VARIABLES
bool        quit = false;
float       deltaTime = 0.0f;    // Keep track of time per frame.
float       lastTime = 0.0f;    // variable to keep overall time.
bool        keyStatus[1024];    // Hold key status.
bool		mouseEnabled = true; // keep track of mouse toggle.

// MAIN GRAPHICS OBJECT
Graphics    myGraphics;        // Runing all the graphics in this object

// DEMO OBJECTS
Cube        myCube;
Sphere      mySphere;
Arrow       arrowX;
Arrow       arrowY;
Arrow       arrowZ;
Cube        myFloor;
Line        myLine;
Cylinder    myCylinder;

// Some global variable to do the animation.
float t = 0.001f;            // Global variable for animation