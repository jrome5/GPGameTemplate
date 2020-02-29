#include "GP_Template.h"
#include "AABB.h"
#include <math.h> 
#include "graph.h"
#include "Astar.h"

constexpr int ROWS = 20;
constexpr int COLS = 20;
constexpr bool grid[ROWS][COLS] = { {0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
									{1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
									{1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0},
									{1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0},
									{0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1},
									{1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0},
									{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
									{0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
									{1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
									{0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0},
									{0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1},
									{0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0},
									{0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
									{1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1},
									{0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0},
									{0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
								}; //create blocks on 1s, empty space on zeros

constexpr int MAZE_SIZE = ROWS * COLS;
constexpr int PHYSICS_DEMO = 57;
constexpr int PARTICLE_DEMO = 342;
constexpr int BOIDS_DEMO = 357;

int target = -1;

AABB camera_aabb;
typedef struct wall
{
	Cube visual;
	AABB aabb;
	glm::vec3 pos;
};
std::vector<wall> inner_walls;
std::vector<wall> outer_walls;
std::vector<Vertex> path;
std::vector<Cube> path_visuals;
Graph graph;
Cube active_cell;
glm::vec3 prev_cell_pos(0.0f, 0.0f, 0.0f);

int main()
{
	int errorGraphics = myGraphics.Init();			// Launch window and graphics context
	if (errorGraphics) return 0;					// Close if something went wrong...
	cout << "Please enter the number of your destination:" << std::endl;
	cout << "1. Physics Demo" << std::endl;
	cout << "2. Particle Demo" << std::endl;
	cout << "3. Boids Demo" << std::endl;

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

wall makeOuterWall(const glm::vec3 pos)
{
	wall w;
	Cube c;
	c.fillColor = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);
	c.Load();
	w.pos = pos;
	w.visual = c;
	AABB aabb;
	aabb.c = Point(pos.x, pos.y, pos.z);
	aabb.r = Point{ 0.5f, 0.5f, 0.5f };
	w.aabb = aabb;
	return w;
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

	myGraphics.cameraPosition = glm::vec3(float(ROWS), 0.5f, float(COLS));
	myGraphics.cameraYaw = 90.05f;
	myGraphics.cameraPitch = -55.05f;

	// Load Geometry examples
	myFloor.Load();
	myFloor.fillColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand Colour
	myFloor.lineColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand again

	active_cell.Load();
	active_cell.fillColor = glm::vec4(0.5f, 0.5f, 1.0f, 0.75f);
	active_cell.lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	createMaze();

	for (int i = 0; i < MAZE_SIZE; i++)
	{
		Cube c;
		c.Load();
		c.fillColor = glm::vec4(1.0f, 1.0f, 0.0f, 0.75f);
		c.lineColor = glm::vec4(1.0f, 1.0f, 0.0f, 0.0f);
		path_visuals.push_back(c);
	}

	camera_aabb.r = Point(myGraphics.cameraPosition.x, myGraphics.cameraPosition.y, myGraphics.cameraPosition.z);
	camera_aabb.r = Point(0.3f, 0.3f, 0.3f);
	// Optimised Graphics
	myGraphics.SetOptimisations();        // Cull and depth testing

}

void createMaze()
{
	//createMaze
	int k = 0;
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			glm::vec3 pos;
			pos.x = float(COLS - j);
			pos.y = 0.5f;
			pos.z = float(ROWS - i);
			const bool create_wall = grid[i][j];
			if (create_wall)
			{
				wall w;
				Cube c;
				c.Load();
				w.visual = c;
				w.pos = pos;
				AABB aabb;
				aabb.c = Point(pos.x, pos.y, pos.z);
				aabb.r = Point(0.5f, 0.5f, 0.5f);
				w.aabb = aabb;
				inner_walls.push_back(w);
			}
			else
			{
				Vertex n;
				n.setID(k);
				n.setPosition(pos.x, pos.z);
				graph.addVertex(n);
			}
			k++;
		}
	}
	//Connect cells
	for (auto p : graph.getVertices())
	{
		int frm = p.first;
		int to = frm + ROWS;
		if ((frm + ROWS) <= (MAZE_SIZE - 1) && graph.checkID(to))
		{
			graph.addEdge(frm, frm + ROWS, 1);
		}
		to = frm + 1;
		if (frm % ROWS != (ROWS - 1) && graph.checkID(to))
		{
			graph.addEdge(frm, frm + 1, 1);
		}
	}
	//for (auto p : graph.getVertices())
	//{
	//	Vertex v = p.second;
	//	cout << std::endl << v.getID() << ": ";
	//	for (auto adjacent : v.getConnections())
	//	{
	//		auto a = adjacent.first;
	//		cout << a << ", ";
	//	}
	//}
		
	//Outer walls
	for (int i = 0; i < ROWS + 1; i++)
	{
		glm::vec3 pos;
		pos.x = float(ROWS - i);
		pos.y = 0.5f;
		pos.z = 0;
		outer_walls.push_back(makeOuterWall(pos)); //top barrier
		pos.z = float(COLS + 1);
		outer_walls.push_back(makeOuterWall(pos)); //bottom barrier
	}
	for (int i = 0; i < COLS + 1; i++)
	{
		glm::vec3 pos;
		pos.x = 0;
		pos.y = 0.5f;
		pos.z = float(COLS - i);
		outer_walls.push_back(makeOuterWall(pos)); //top barrier
		pos.x = float(COLS + 1);
		outer_walls.push_back(makeOuterWall(pos)); //bottom barrier
	}
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

	glm::vec3 next_pos = myGraphics.cameraPosition;

	// Update movement using the keys
	GLfloat cameraSpeed = 2.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) next_pos += cameraSpeed * myGraphics.cameraFront;
	if (keyStatus[GLFW_KEY_S]) next_pos -= cameraSpeed * myGraphics.cameraFront;
	if (keyStatus[GLFW_KEY_A]) next_pos -= glm::normalize(glm::cross(myGraphics.cameraFront, myGraphics.cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) next_pos += glm::normalize(glm::cross(myGraphics.cameraFront, myGraphics.cameraUp)) * cameraSpeed;
	
	next_pos.y = 0.5f; //fixed height;
	camera_aabb.c = Point(next_pos.x, next_pos.y, next_pos.z);

	bool collision = false;
	for (auto wall : inner_walls)
	{
		if (isCollisionSphere(camera_aabb, wall.aabb))
		{
			collision = true;
			break;
		}
	}
	if (not collision) {

		for (auto wall : outer_walls)
		{
			if (isCollisionSphere(camera_aabb, wall.aabb))
			{
				collision = true;
				break;
			}
		}
	}
	if (!collision)
	{
		//update camera
		myGraphics.cameraPosition = next_pos;
	}
	else
	{
		camera_aabb.c = Point{ myGraphics.cameraPosition.x, myGraphics.cameraPosition.y, myGraphics.cameraPosition.z };
	}

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
	for (auto& wall: inner_walls)
	{
		// Calculate Cube position
		glm::mat4 mv_matrix_cube =
			glm::translate(glm::vec3(wall.pos.x, wall.pos.y, wall.pos.z)) *
			glm::scale(glm::vec3(1.0f, 1.0f, 1.0f))*
			glm::mat4(1.0f);
		wall.visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube;
		wall.visual.proj_matrix = myGraphics.proj_matrix;
	}

	for (auto& wall : outer_walls)
	{
		// Calculate Cube position
		glm::mat4 mv_matrix_cube =
			glm::translate(glm::vec3(wall.pos.x, wall.pos.y, wall.pos.z)) *
			glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)) *
			glm::mat4(1.0f);
		wall.visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_cube;
		wall.visual.proj_matrix = myGraphics.proj_matrix;
	}
		
	// Calculate floor position and resize
	myFloor.mv_matrix = myGraphics.viewMatrix *
		glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::scale(glm::vec3(1000.0f, 0.001f, 1000.0f)) *
		glm::mat4(1.0f);
	myFloor.proj_matrix = myGraphics.proj_matrix;
	
	bool new_path = false;
	// Calculate floor position and resize
	if (keyStatus[GLFW_KEY_1]) 
	{
		target = PHYSICS_DEMO;
		new_path = true;
	}
	if (keyStatus[GLFW_KEY_2])
	{
		target = PARTICLE_DEMO;
		new_path = true;

	}
	if (keyStatus[GLFW_KEY_3])
	{
		target = BOIDS_DEMO;
		new_path = true;
	}

	glm::vec3 cell_pos;
	cell_pos.x = roundf(myGraphics.cameraPosition.x);
	cell_pos.y = 0.5f;
	cell_pos.z = roundf(myGraphics.cameraPosition.z);
	
	if ((cell_pos != prev_cell_pos) or new_path)
	{
		//cout << "New path";
		if (target != -1)
		{
			path.clear();
			int start = (ROWS - cell_pos.z) * ROWS + (COLS - cell_pos.x);
			a_star_search(graph, start, target, path);
		}
		prev_cell_pos = cell_pos;
	}

	active_cell.mv_matrix = myGraphics.viewMatrix *
		glm::translate(glm::vec3(cell_pos)) *
		glm::scale(glm::vec3(1.0f, 0.5f, 1.0f)) *
		glm::mat4(1.0f);
	active_cell.proj_matrix = myGraphics.proj_matrix;

	for (int i = 0; i < MAZE_SIZE; i++)
	{
		Cube& c = path_visuals[i];
		if (i < path.size())
		{
			Vertex p = path[i];
			auto pos = p.getPosition();
			c.mv_matrix = myGraphics.viewMatrix *
				glm::translate(glm::vec3(pos.x, 0.0f, pos.y)) *
				glm::scale(glm::vec3(1.0f, 0.01f, 1.0f)) *
				glm::mat4(1.0f);
			c.proj_matrix = myGraphics.proj_matrix;
		}
		else //create null
		{
			c.mv_matrix = myGraphics.viewMatrix *
				glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
				glm::scale(glm::vec3(0.0f, 0.0f, 0.0f)) *
				glm::mat4(1.0f);
			c.proj_matrix = myGraphics.proj_matrix;
		}
	}
	if (glfwWindowShouldClose(myGraphics.window) == GL_TRUE) quit = true; // If quit by pressing x on window.
}

void renderScene() {
	// Clear viewport - start a new frame.
	myGraphics.ClearViewport();
	myFloor.Draw();
	// Draw objects in screen
	for (auto& wall : inner_walls)
	{
		wall.visual.Draw();
	}
	for (auto& wall : outer_walls)
	{
		wall.visual.Draw();
	}
	for (auto& p : path_visuals)
	{
		p.Draw();
	}
	active_cell.Draw();
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
	/*	cout << "Camera up:" << myGraphics.cameraUp.x << " " << myGraphics.cameraUp.y << " " << myGraphics.cameraUp.z << "\n";
		cout << "Camera pitch:" << myGraphics.cameraPitch << "\n";
		cout << "Camera yaw:" << myGraphics.cameraYaw << "\n";
		cout << "Camera pos:" << myGraphics.cameraPosition.x << " " << myGraphics.cameraPosition.y << " " << myGraphics.cameraPosition.z << "/n";*/
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
