#include "GP_Template.h"
#include "bounding_box.h"
#include "physics.h"
#include "particle.h"
#include <algorithm>
bounding_box::BoundingBox b({ 3.0f, 10.0f, 3.0f }, 5);
Cube emitter_visual;
Emitter emitter;
std::vector<Cube> particle_visuals;
constexpr int number_of_particles = 360;

int main()
{
	int errorGraphics = myGraphics.Init();			// Launch window and graphics context
	if (errorGraphics) return 0;					// Close if something went wrong...

	startup();										// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	// MAIN LOOP run until the window is closed
	while (!quit) {

		// Update the camera transform based on interactive inputs or by following a predifined path.
	
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
	myFloor.Load();
	myFloor.fillColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand Colour
	myFloor.lineColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand again

	//BoundingBox
	b.visual.Load();
	emitter.spawn(glm::vec3{ 0.0f, 0.0f, 0.0f }, number_of_particles);
	emitter_visual.Load();
	emitter_visual.fillColor = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
	for (int i = 0; i < number_of_particles; i++)
	{
		Cube s;
		s.Load();
		particle_visuals.push_back(s);
	}

	// Optimised Graphics
	myGraphics.SetOptimisations();        // Cull and depth testing

}

void updateSceneElements() {

	glfwPollEvents();                                // poll callbacks

	// Calculate frame time/period -- used for all (physics, animation, logic, etc).
	GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
	deltaTime = currentTime - lastTime;                // Calculate delta time
	lastTime = currentTime;                            // Save for next frame calculations.

	emitter_visual.mv_matrix = myGraphics.viewMatrix *
		glm::translate(glm::vec3(emitter.getPosition())) *
		glm::scale(glm::vec3(1.0f,1.0f, 1.0f)) *
		glm::mat4(1.0f);
	emitter_visual.proj_matrix = myGraphics.proj_matrix;

		
	// Calculate floor position and resize
	myFloor.mv_matrix = myGraphics.viewMatrix *
		glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::scale(glm::vec3(1000.0f, 0.001f, 1000.0f)) *
		glm::mat4(1.0f);
	myFloor.proj_matrix = myGraphics.proj_matrix;

	if (glfwWindowShouldClose(myGraphics.window) == GL_TRUE) quit = true; // If quit by pressing x on window.
}

void update(const float current_time)
{
	const auto dt = std::min(deltaTime, 0.2f);  //TODO: remove this workaround
	const float magnitude = 5.0f;
	for (int i = 0; i < number_of_particles; i++)
	{
		Particle& particle = emitter.getParticle(i);

		if (not particle.checkExpired(dt))
		{
			glm::vec3 accel;
			accel.x = physics::getRandomFloat(magnitude, -magnitude/2);
			accel.y = physics::getRandomFloat(0.5f, 0);
			accel.z = physics::getRandomFloat(magnitude, -magnitude/2);

			particle.velocity += physics::calculateVelocity(accel, dt);
			particle.position += physics::calculatePosition(particle.velocity, dt);
		}

		auto& visual = particle_visuals[i];

		glm::mat4 mv_matrix_sphere =
			glm::translate(particle.position) *
			glm::scale(glm::vec3(0.01f,0.01f,0.01f))*
			glm::mat4(1.0f);
		visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_sphere;
		visual.proj_matrix = myGraphics.proj_matrix;
	}

	if (b.getPosition().y > b.getScale().y)
	{
		const auto force = physics::calculateForce(gravity, b.mass);
		const auto acceleration = physics::calculateAcceleration(force, b.mass);
		b.velocity += physics::calculateVelocity(acceleration, dt);
		b.position = b.position + physics::calculatePosition(b.getVelocity(), dt);
	}

	//check if bounding box visuals
	glm::mat4 mv_matrix_cube =
		glm::translate(b.getPosition()) *
		glm::mat4(1.0f);
	b.visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube;
	b.visual.proj_matrix = myGraphics.proj_matrix;
}

void renderScene() {
	// Clear viewport - start a new frame.
	myGraphics.ClearViewport();
	myFloor.Draw();
	b.visual.Draw();
	emitter_visual.Draw();

	for (auto& p : particle_visuals)
	{
		p.Draw();
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
