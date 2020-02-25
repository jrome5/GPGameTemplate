#include "GP_Template.h"
#include "bounding_box.h"
#include <algorithm>
#include "boid.h"

std::vector<Cube> boid_visuals;
constexpr int number_of_boids = 200;
std::vector<Boid> boids;
constexpr float MAX_SPEED = 0.1f;
constexpr float MAX_FORCE = 0.05f;
constexpr float VISION_RADIUS = 2.5f;
float al = 1.0f;
float sep = 1.0f;
float coh = 1.0f;
float fol = 2.0f;

int main()
{
	int errorGraphics = myGraphics.Init();			// Launch window and graphics context
	if (errorGraphics) return 0;					// Close if something went wrong...

	startup();										// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	// MAIN LOOP run until the window is closed
	while (!quit) {

		// Update the camera transform based on interactive inputs or by following a predifined path.
		updateCamera();
		// Update position, orientations and any other relevant visual state of any dynamic elements in the scene.
		updateSceneElements();

		const float current_time = float(glfwGetTime());
		update(current_time);
		// Render a still frame into an off-screen frame buffer known as the backbuffer.
		renderScene();

		// Swap the back buffer with the front buffer, making the most recently rendered image visible on-screen.
		glfwSwapBuffers(myGraphics.window);        // swap buffers (avoid flickering and tearing)

	}


	myGraphics.endProgram();            // Close and clean everything up...

   // cout << "\nPress any key to continue...\n";
   // cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void startup() {
	// Keep track of the running time
	GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
	deltaTime = currentTime;                        // start delta time
	lastTime = currentTime;                            // Save for next frame calculations.

	// Callback graphics and key update functions - declared in main to avoid scoping complexity.
	// More information here : https://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowSizeCallback(myGraphics.window, onResizeCallback);            // Set callback for resize
	glfwSetKeyCallback(myGraphics.window, onKeyCallback);                    // Set Callback for keys
	glfwSetMouseButtonCallback(myGraphics.window, onMouseButtonCallback);    // Set callback for mouse click
	glfwSetCursorPosCallback(myGraphics.window, onMouseMoveCallback);        // Set callback for mouse move
	glfwSetScrollCallback(myGraphics.window, onMouseWheelCallback);            // Set callback for mouse wheel.

	// Calculate proj_matrix for the first time.
	myGraphics.aspect = (float)myGraphics.windowWidth / (float)myGraphics.windowHeight;
	myGraphics.proj_matrix = glm::perspective(glm::radians(50.0f), myGraphics.aspect, 0.1f, 1000.0f);

	//camera (constants set)
	myGraphics.cameraPitch = -72.0f;
	myGraphics.cameraYaw = -89.0f;
	myGraphics.cameraPosition = glm::vec3(4.32f, 12.74f, 0.66f);
	myGraphics.cameraFront = glm::vec3(0.000462104f, -0.964326f, 0.264716f);
	myGraphics.cameraUp = glm::vec3(0, 1, 0);
	myGraphics.viewMatrix = glm::lookAt(myGraphics.cameraPosition,			// eye
		myGraphics.cameraPosition + myGraphics.cameraFront,					// centre
		myGraphics.cameraUp);												// up

	// Load Geometry examples
	//myCube.Load();
	//myCube.fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	myFloor.Load();
	myFloor.fillColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand Colour
	myFloor.lineColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand again

	for (int i = 0; i < number_of_boids; i++)
	{

		Cube s;
		s.Load();
		if (i == 0) {
			s.fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		boid_visuals.push_back(s);
		Boid b(MAX_SPEED, MAX_FORCE, VISION_RADIUS);
		boids.push_back(b);
	}

	// Optimised Graphics
	myGraphics.SetOptimisations();        // Cull and depth testing
}

void updateCamera() {

	// calculate movement for FPS camera
	GLfloat xoffset = myGraphics.mouseX - myGraphics.cameraLastX;
	GLfloat yoffset = myGraphics.cameraLastY - myGraphics.mouseY;    // Reversed mouse movement
	myGraphics.cameraLastX = (GLfloat)myGraphics.mouseX;
	myGraphics.cameraLastY = (GLfloat)myGraphics.mouseY;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	myGraphics.cameraYaw += xoffset;
	myGraphics.cameraPitch += yoffset;

	// check for pitch out of bounds otherwise screen gets flipped
	if (myGraphics.cameraPitch > 89.0f) myGraphics.cameraPitch = 89.0f;
	if (myGraphics.cameraPitch < -89.0f) myGraphics.cameraPitch = -89.0f;

	// Calculating FPS camera movement (See 'Additional Reading: Yaw and Pitch to Vector Calculations' in VISION)
	glm::vec3 front;
	front.x = cos(glm::radians(myGraphics.cameraYaw)) * cos(glm::radians(myGraphics.cameraPitch));
	front.y = sin(glm::radians(myGraphics.cameraPitch));
	front.z = sin(glm::radians(myGraphics.cameraYaw)) * cos(glm::radians(myGraphics.cameraPitch));

	myGraphics.cameraFront = glm::normalize(front);

	// Update movement using the keys
	GLfloat cameraSpeed = 10.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) myGraphics.cameraPosition += cameraSpeed * myGraphics.cameraFront;
	if (keyStatus[GLFW_KEY_S]) myGraphics.cameraPosition -= cameraSpeed * myGraphics.cameraFront;
	if (keyStatus[GLFW_KEY_A]) myGraphics.cameraPosition -= glm::normalize(glm::cross(myGraphics.cameraFront, myGraphics.cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) myGraphics.cameraPosition += glm::normalize(glm::cross(myGraphics.cameraFront, myGraphics.cameraUp)) * cameraSpeed;

	// IMPORTANT PART
	// Calculate my view matrix using the lookAt helper function
	if (mouseEnabled) {
		myGraphics.viewMatrix = glm::lookAt(myGraphics.cameraPosition,			// eye
			myGraphics.cameraPosition + myGraphics.cameraFront,					// centre
			myGraphics.cameraUp);												// up
	}
}

void updateSceneElements() {

	glfwPollEvents();                                // poll callbacks

	// Calculate frame time/period -- used for all (physics, animation, logic, etc).
	GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
	deltaTime = currentTime - lastTime;                // Calculate delta time
	lastTime = currentTime;                            // Save for next frame calculations.

	// Calculate floor position and resize
	myFloor.mv_matrix = myGraphics.viewMatrix *
		glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::scale(glm::vec3(1000.0f, 0.001f, 1000.0f)) *
		glm::mat4(1.0f);
	myFloor.proj_matrix = myGraphics.proj_matrix;

	// Calculate Cube position
	//glm::mat4 mv_matrix_cube =
	//	glm::translate(glm::vec3(10.0f, 10.0f, 10.0f)) *
	//	glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)) *
	//	glm::mat4(1.0f);
	//myCube.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube;
	//myCube.proj_matrix = myGraphics.proj_matrix;




	if (glfwWindowShouldClose(myGraphics.window) == GL_TRUE) quit = true; // If quit by pressing x on window.
}

void update(const float current_time)
{
	if (keyStatus[GLFW_KEY_U]) al += 0.1f;
	if (keyStatus[GLFW_KEY_J]) al -= 0.1f;
	if (keyStatus[GLFW_KEY_I]) sep += 0.1f;
	if (keyStatus[GLFW_KEY_K]) sep -= 0.1f;
	if (keyStatus[GLFW_KEY_O]) coh += 0.1f;
	if (keyStatus[GLFW_KEY_L]) coh -= 0.1f;

	const float dt = std::min(deltaTime, 1.0f);
	for (int i = 0; i < number_of_boids; i++)
	{
		Boid& boid = boids[i];
		auto& visual = boid_visuals[i];
		boid.behaviour(boids, boids[0].position , al, sep, coh, fol);
		//boid.behaviour(boids, myGraphics.cameraPosition, al, sep, coh, fol);

		//boid.cageJack();
		boid.cageShayne();
		boid.update(dt);
		

		if (i == 0) {

			glm::mat4 mv_matrix_sphere =
				glm::translate(boid.position) *
				glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)) *
				glm::mat4(1.0f);
			visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_sphere;
			visual.proj_matrix = myGraphics.proj_matrix;

		}

		glm::mat4 mv_matrix_sphere =
			glm::translate(boid.position) *
			glm::scale(glm::vec3(0.1f,0.1f,0.1f))*
			glm::mat4(1.0f);
		visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_sphere;
		visual.proj_matrix = myGraphics.proj_matrix;
	}
}

void renderScene() {
	// Clear viewport - start a new frame.
	myGraphics.ClearViewport();
	myFloor.Draw();
	//myCube.Draw();

	for (auto & boid : boid_visuals)
	{
		boid.Draw();
	}
}

// CallBack functions low level functionality.
void onResizeCallback(GLFWwindow* window, int w, int h) {    // call everytime the window is resized
	//myGraphics.windowWidth = w;
	//myGraphics.windowHeight = h;

	glfwGetFramebufferSize(window, &myGraphics.windowWidth, &myGraphics.windowHeight);

	myGraphics.aspect = (float)w / (float)h;
	myGraphics.proj_matrix = glm::perspective(glm::radians(50.0f), myGraphics.aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) { // called everytime a key is pressed
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	// toggle showing mouse.
	if (keyStatus[GLFW_KEY_M]) {
		mouseEnabled = !mouseEnabled;
		myGraphics.ToggleMouse();
		cout << "al: " << al << std::endl;
		cout << "sep: " << sep << std::endl;
		cout << "coh: " << coh << std::endl;

	}
	// If exit key pressed.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	myGraphics.mouseX = mouseX;
	myGraphics.mouseY = mouseY;

	// helper variables for FPS camera
	if (myGraphics.cameraFirstMouse) {
		myGraphics.cameraLastX = (GLfloat)myGraphics.mouseX; myGraphics.cameraLastY = (GLfloat)myGraphics.mouseY; myGraphics.cameraFirstMouse = false;
	}
}

void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);
}
