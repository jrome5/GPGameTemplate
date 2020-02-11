#include "GP_Template.h"
#include "AABB.h"

constexpr bool grid[10][10] = { {0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
								{0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
								{1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
								{1, 0, 0, 0, 1, 0, 0, 1, 0, 1},
								{1, 0, 1, 1, 0, 0, 0, 1, 0, 1},
								{1, 0, 1, 0, 0, 0, 0, 1, 1, 1},
								{1, 0, 1, 0, 0, 1, 0, 0, 0, 1},
								{1, 0, 1, 1, 1, 0, 0, 0, 0, 0},
								{1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
								{1, 1, 1, 1, 1, 1, 1, 1, 0, 1} }; //create blocks on 1s, empty space on zeros

constexpr int ROWS = 10;
constexpr int COLS = 10;

struct character
{
	Sphere shape;
	GLfloat pos_x = 0.0f;
	GLfloat pos_y = 0.5f;
	GLfloat pos_z = 0.0f;
	GLfloat roll = 0.0f;
	GLfloat pitch = 0.0f;
	GLfloat yaw = 0.0f;

} myCharacter;

AABB character_aabb;
std::vector<AABB> wall_aabbs;
std::vector<Cube> walls;

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

	// Load Geometry examples
	myFloor.Load();
	myFloor.fillColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand Colour
	myFloor.lineColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand again

	//createMaze
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			const bool create_wall = grid[i][j];
			if (create_wall)
			{
				Cube wall;
				wall.Load();
				walls.push_back(wall);
				AABB aabb;
				aabb.c = Point(-500.0f, -500.0f, -500.0f);
				aabb.r = Point(0.5f, 0.5f, .5f);
				wall_aabbs.push_back(aabb);
			}
		}
	}
	//character
	myCharacter.shape.Load();
	myCharacter.shape.fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	character_aabb.r = Point(myCharacter.pos_x, myCharacter.pos_y, myCharacter.pos_z);
	character_aabb.r = Point(0.4, 0.5f, 0.4f);


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

void updateSceneElements() {

	glfwPollEvents();                                // poll callbacks

	// Calculate frame time/period -- used for all (physics, animation, logic, etc).
	GLfloat currentTime = (GLfloat)glfwGetTime();    // retrieve timelapse
	deltaTime = currentTime - lastTime;                // Calculate delta time
	lastTime = currentTime;                            // Save for next frame calculations.

	//createMaze
	int k = 0;
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			const bool wall_exists = grid[i][j];
			if (wall_exists)
			{
				Cube& wall = walls[k];
				// Calculate Cube position
				const auto cube_size = 1.0f;
				const auto x_pos = i * cube_size;
				const auto y_pos = 0.5f;
				const auto z_pos = j * cube_size;

				glm::mat4 mv_matrix_cube =
					glm::translate(glm::vec3(x_pos, y_pos, z_pos)) *
					glm::mat4(1.0f);
				wall.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube;
				wall.proj_matrix = myGraphics.proj_matrix;
				wall_aabbs[k].c = Point(x_pos, y_pos, z_pos);
				k += 1;
			}
		}
	}
		
	// Calculate floor position and resize
	myFloor.mv_matrix = myGraphics.viewMatrix *
		glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::scale(glm::vec3(1000.0f, 0.001f, 1000.0f)) *
		glm::mat4(1.0f);
	myFloor.proj_matrix = myGraphics.proj_matrix;

	GLfloat x = 0.0f;
	GLfloat z = 0.0f;
	GLfloat speed = 0.01f;

	if (keyStatus[GLFW_KEY_UP]) z += speed;
	if (keyStatus[GLFW_KEY_DOWN]) z -= speed;
	if (keyStatus[GLFW_KEY_LEFT]) x += speed;
	if (keyStatus[GLFW_KEY_RIGHT]) x -= speed;
	moveCharacter(x, 0.0f, z);


	if (glfwWindowShouldClose(myGraphics.window) == GL_TRUE) quit = true; // If quit by pressing x on window.

}

void moveCharacter(const GLfloat x, const GLfloat y, const GLfloat z)
{
	//calculate displacements
	GLfloat new_x = myCharacter.pos_x + x;
	GLfloat new_y = myCharacter.pos_y + y;
	GLfloat new_z = myCharacter.pos_z + z;

	GLfloat new_roll = myCharacter.roll + z;
	GLfloat new_pitch = myCharacter.pitch + x;
	GLfloat new_yaw = myCharacter.yaw + y;

	character_aabb.c = Point(new_x, new_y, new_z);

	bool collision = false;
	for (int i = 0; i < wall_aabbs.size(); i++)
	{
		if (isCollision(character_aabb, wall_aabbs[i]))
		{
			collision = true;
			break;
		}
	}
	if (collision)
	{
		new_x = myCharacter.pos_x;
		new_y = myCharacter.pos_y;
		new_z = myCharacter.pos_z;

		new_roll = myCharacter.roll;
		new_pitch = myCharacter.pitch;
		new_yaw = myCharacter.yaw;
		character_aabb.c = Point(new_x, new_y, new_z);
	}
	else
	{
		//update Character
		myCharacter.pos_x = new_x;
		myCharacter.pos_y = new_y;
		myCharacter.pos_z = new_z;
		myCharacter.roll = new_roll;
		myCharacter.pitch = new_pitch;
		myCharacter.yaw = new_yaw;
	}

	//translate and rotate accordingly
	glm::mat4 mv_matrix_character =
		glm::translate(glm::vec3(new_x, new_y, new_z)) *
		glm::rotate(new_roll, glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::rotate(new_pitch, glm::vec3(0.0f, 0.0f, -1.0f)) *
		glm::rotate(new_yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::mat4(1.0f);
	myCharacter.shape.mv_matrix = myGraphics.viewMatrix * mv_matrix_character;
	myCharacter.shape.proj_matrix = myGraphics.proj_matrix;
}

void renderScene() {
	// Clear viewport - start a new frame.
	myGraphics.ClearViewport();
	myFloor.Draw();
	// Draw objects in screen
	for (auto& wall : walls)
	{
		wall.Draw();
	}
	myCharacter.shape.Draw();
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
