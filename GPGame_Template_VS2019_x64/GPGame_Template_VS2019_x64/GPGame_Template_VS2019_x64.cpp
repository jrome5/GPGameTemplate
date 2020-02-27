#include "GP_Template.h"
#include <algorithm>

BoundingBox cube1({ 3.0f, 5.0f, 3.0f }, 5, 0);
BoundingBox cube2({ 3.0f, 3.0f, 5.0f }, 5, 1);
BoundingBox myFloor({ 0.0f, 0.0f, 0.0f }, 1000.0f, 2);
std::vector<BoundingBox> b_boxes;
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
	//myGraphics.cameraPitch = -72.0f;
	//myGraphics.cameraYaw = -89.0f;
	//myGraphics.cameraPosition = glm::vec3(4.32f, 12.74f, 0.66f);
	//myGraphics.cameraFront = glm::vec3(0.000462104f, -0.964326f, 0.264716f);
	myGraphics.cameraUp = glm::vec3(0, 1, 0);
	myGraphics.viewMatrix = glm::lookAt(myGraphics.cameraPosition,			// eye
		myGraphics.cameraPosition + myGraphics.cameraFront,					// centre
		myGraphics.cameraUp);												// up
	// Load Geometry examples
	myFloor.visual.Load();
	myFloor.visual.fillColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand Colour
	myFloor.visual.lineColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand again
	myFloor.setScale(1000.f, 0.005f, 1000.0f);
	myFloor.is_static = true;

	//BoundingBox
	cube1.visual.Load();
	cube1.visual.fillColor = glm::vec4(0.5f, 0.5f, 0.9f, 1.0f);
	cube1.setScale(1.0f, 1.0f, 1.0f);
	cube2.visual.Load();
	cube2.setScale(1.0f, 1.0f, 1.0f);

	//Add b boxes to vector
	b_boxes.push_back(myFloor);
	b_boxes.push_back(cube1);
	b_boxes.push_back(cube2);
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
	GLfloat cameraSpeed = 1.0f * deltaTime;
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

inline glm::vec3 mulVector(glm::vec3 v1, float m)
{
	glm::vec3 v2;
	v2.x = v1.x * m;
	v2.y = v1.y * m;
	v2.z = v1.z * m;
	return v2;
}


inline glm::vec3 limitVector(glm::vec3 v1, float m)
{
	glm::vec3 v2;
	v2.x = (abs(v1.x) < m) ? 0.0f : v1.x;
	v2.y = (abs(v1.y) < m) ? 0.0f : v1.y;
	v2.z = (abs(v1.z) < m) ? 0.0f : v1.z;
	return v2;
}


void calculateResponse(BoundingBox& b1, BoundingBox& b2)
{
	const float velocity_cutoff = 0.05f; //stop vibration due to bouncing at low velocity
	glm::vec3 v1;
	glm::vec3 v2;
	auto m1 = b1.mass;
	auto m2 = b2.mass;
	auto u1 = b1.velocity;
	auto u2 = b2.velocity;
	if (not b1.is_static)
	{
		v1 = ((m1 - m2) / (m1 + m2)) * u1 + (2 * m2 / (m1 + m2)) * u2;
		v1 = mulVector(v1, b2.friction);
		//v1 = limitVector(v1, velocity_cutoff);
		b1.velocity = v1;
	}
	if (not b2.is_static)
	{
		v2 = ((m2 - m1) / (m1 + m2)) * u2 + (2 * m1 / (m1 + m2)) * u1;
		v2 = mulVector(v2, b1.friction);
		//v2 = limitVector(v2, velocity_cutoff);
		b2.velocity = v2;
	}
	//momentum calculations
}

void updateSceneElements() {

	glfwPollEvents();                                // poll callbacks

	// Calculate frame time/period -- used for all (physics, animation, logic, etc).
	GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
	deltaTime = currentTime - lastTime;                // Calculate delta time
	lastTime = currentTime;                            // Save for next frame calculations.
	
	const float dt = std::min(deltaTime, 0.2f);

	for (auto& b : b_boxes)
	{
		if (not b.is_static)
		{
			const auto force = b.calculateForce(gravity);
			const auto acceleration = b.calculateAcceleration(force);
			b.calculateVelocity(acceleration, dt);
		}
		for (auto& b2 : b_boxes)
		{
			if (b.id == b2.id)
				continue;
			else if (isCollision(b.aabb, b2.aabb))
			{
				calculateResponse(b, b2);
				updateBoundingBox(b2);
			}
		}
		updateBoundingBox(b);
	}

	if (glfwWindowShouldClose(myGraphics.window) == GL_TRUE) quit = true; // If quit by pressing x on window.
}

void updateBoundingBox(BoundingBox& b)
{
	const float dt = std::min(deltaTime, 0.2f);
	b.calculatePosition(b.velocity, dt);
	b.setPosition(b.position);
	//check if bounding box visuals
	glm::mat4 mv_matrix_cube =
		glm::translate(b.position) *
		glm::scale(b.scale)*
		glm::mat4(1.0f);
	b.visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube;
	b.visual.proj_matrix = myGraphics.proj_matrix;
}

void renderScene() {
	// Clear viewport - start a new frame.
	myGraphics.ClearViewport();
	for (auto& v : b_boxes)
		v.visual.Draw();
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
