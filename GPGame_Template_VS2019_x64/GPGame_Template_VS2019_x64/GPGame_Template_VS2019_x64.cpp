#include "GP_Template.h"
#include "AABB.h"
#include <math.h> 
#include "graph.h"
#include "Astar.h"
#include "bounding_box.h"
#include "boid.h"

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
//demo location, activation point
constexpr std::pair<int, int> PARTICLE_DEMO = std::make_pair(57, 157); 
constexpr std::pair<int, int> PHYSICS_DEMO = std::make_pair(342, 242);
constexpr std::pair<int, int> BOIDS_DEMO = std::make_pair(357, 237);

int target = -1;
int start = -1;
int activation_point = -1;
bool activate_demo = false;

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
BoundingBox myFloor({ 0.0f, 0.0f, 0.0f }, 1000.0f, 2);

//PHYSICS DEMO
BoundingBox cube1({ 18.0f, 3.0f, 3.0f }, 2.0f, 0);
BoundingBox cube2({ 18.0f, 12.0f, 4.1f }, 5.0f, 1);
std::vector<BoundingBox> b_boxes;

//BOIDS_DEMO
std::vector<Cube> boid_visuals;
Cube container;
constexpr int number_of_boids = 150;
std::vector<Boid> boids;
constexpr float CONTAINER_SIZE = 5.0f;
constexpr float MAX_SPEED = 0.05f;
constexpr float MAX_FORCE = 0.01f;
constexpr float VISION_RADIUS = 0.25f;
constexpr float BOID_SIZE = 0.05f;
float al = 1.0f;
float sep = 1.0f;
float coh = 1.0f;
float fol = 0.0f;


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
	aabb.c = pos;
	aabb.r = glm::vec3{ 0.5f, 0.5f, 0.5f };
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
	myFloor.visual.Load();
	myFloor.visual.fillColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand Colour
	myFloor.visual.lineColor = glm::vec4(130.0f / 255.0f, 96.0f / 255.0f, 61.0f / 255.0f, 1.0f);    // Sand again
	myFloor.setScale(1000.f, 0.005f, 1000.0f);
	myFloor.is_static = true;
//	myFloor.friction = 0.75;

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

	//BOIDS
	container.Load();
	container.fillColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	container.lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);    // Sand again

	for (int i = 0; i < number_of_boids; i++)
	{

		Cube s;
		s.Load();
		if (i == 0) {
			s.fillColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		}

		boid_visuals.push_back(s);
		Boid b(MAX_SPEED, MAX_FORCE, VISION_RADIUS, CONTAINER_SIZE, i);
		boids.push_back(b);
	}
	
	//maze
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

	camera_aabb.c = myGraphics.cameraPosition;
	const float player_size = 0.25f;
	camera_aabb.r = glm::vec3(player_size, player_size, player_size);
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
				aabb.c = pos;
				aabb.r = glm::vec3(0.5f, 0.5f, 0.5f);
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
	camera_aabb.c = next_pos;

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
	if (not collision)
	{
		for(auto box : b_boxes)
			if (isCollisionSphere(camera_aabb, box.aabb) && box.position.y <= camera_aabb.c.y)
			{
				collision = true;
				break;
			}
	}
	if (!collision)
	{
		//update camera
		myGraphics.cameraPosition = next_pos;
	}
	else
	{
		camera_aabb.c = myGraphics.cameraPosition;
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
	myFloor.visual.mv_matrix = myGraphics.viewMatrix *
		glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)) *
		glm::scale(glm::vec3(1000.0f, 0.001f, 1000.0f)) *
		glm::mat4(1.0f);
	myFloor.visual.proj_matrix = myGraphics.proj_matrix;
	
	bool new_path = false;
	// Calculate floor position and resize
	if (keyStatus[GLFW_KEY_1]) 
	{
		target = PHYSICS_DEMO.first;
		activation_point = PHYSICS_DEMO.second;
		new_path = true;
	}
	if (keyStatus[GLFW_KEY_2])
	{
		target = PARTICLE_DEMO.first;
		activation_point = BOIDS_DEMO.second;
		new_path = true;
	}
	if (keyStatus[GLFW_KEY_3])
	{
		target = BOIDS_DEMO.first;
		activation_point = BOIDS_DEMO.second;
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
			start = (ROWS - cell_pos.z) * ROWS + (COLS - cell_pos.x);
			a_star_search(graph, start, target, path);
			if (start == activation_point)
			{
				activate_demo = !activate_demo;
			}
		}
		prev_cell_pos = cell_pos;
	}

	if (activate_demo)
	{
		switch (activation_point)
		{
		case PHYSICS_DEMO.second:
			updatePhysicsDemo();
			break;
		case PARTICLE_DEMO.second:
			updateParticleDemo();
			break;
		case BOIDS_DEMO.second:
			updateBoidsDemo();
			break;
		default:
			break;
		}
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

void updateBoidsDemo() 
{
	//std::cout << "Me and the boids having a demo" << std::endl;
	if (keyStatus[GLFW_KEY_U]) al += 0.1f;
	if (keyStatus[GLFW_KEY_J]) al -= 0.1f;
	if (keyStatus[GLFW_KEY_I]) sep += 0.1f;
	if (keyStatus[GLFW_KEY_K]) sep -= 0.1f;
	if (keyStatus[GLFW_KEY_O]) coh += 0.1f;
	if (keyStatus[GLFW_KEY_L]) coh -= 0.1f;

	const float dt = std::min(deltaTime, 1.0f);
	for (int i = 0; i < number_of_boids; i++)
	{
		auto& boid = boids[i];
		auto& visual = boid_visuals[i];
		boid.behaviour(boids, boids[0].position, al, sep, coh, fol);
		//boid.behaviour(boids, myGraphics.cameraPosition, al, sep, coh, fol);

	//	boid.cageJack();
		boid.cageShayne();
		boid.update(dt);


		if (i == 0) {

			glm::mat4 mv_matrix_sphere =
				glm::translate(boid.position) *
				glm::scale(glm::vec3(BOID_SIZE, BOID_SIZE, BOID_SIZE)) *
				glm::mat4(1.0f);
			visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_sphere;
			visual.proj_matrix = myGraphics.proj_matrix;

		}

		glm::mat4 mv_matrix_sphere =
			glm::translate(boid.position) *
			glm::scale(glm::vec3(BOID_SIZE, BOID_SIZE, BOID_SIZE)) *
			glm::mat4(1.0f);
		visual.mv_matrix = myGraphics.viewMatrix * mv_matrix_sphere;
		visual.proj_matrix = myGraphics.proj_matrix;
	}

	container.mv_matrix = myGraphics.viewMatrix *
		glm::translate(glm::vec3(3.0f, CONTAINER_SIZE/2, 3.0f)) *
		glm::scale(glm::vec3(CONTAINER_SIZE, CONTAINER_SIZE, CONTAINER_SIZE)) *
		glm::mat4(1.0f);
	container.proj_matrix = myGraphics.proj_matrix;

};
void updatePhysicsDemo() 
{
//	std::cout << "Physics physics" << std::endl;
	const float dt = std::min(deltaTime, 0.2f);

	for (auto& b : b_boxes)
	{
		if (isCollisionSphere(camera_aabb, b.aabb))
		{
			b.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			continue;
		}
		if (not b.is_static && not b.at_rest)
		{
			const auto force = b.calculateForce(gravity);
			const auto acceleration = b.calculateAcceleration(force);
			b.calculateVelocity(acceleration, dt);
		}
		else
		{
			for (auto& b2 : b_boxes)
			{
				if (b.id == b2.id)
					continue;
				if (isCollisionCube(b.aabb, b2.aabb))
				{
					b.calculateResponse(b2);
					b2.update(dt);
				}
			}
		}
		b.update(dt);
	}
};
void updateParticleDemo() 
{
	std::cout << "Partly working" << std::endl;
};

void renderScene() {
	// Clear viewport - start a new frame.
	myGraphics.ClearViewport();
	myFloor.visual.Draw();
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
	if (activate_demo)
	{
		switch (activation_point)
		{
		case PHYSICS_DEMO.second:
			for (auto& v : b_boxes)
			{
				if (v.id == 2)
					continue;
				v.visual.Draw();
			}
			break;
		case PARTICLE_DEMO.second:
			break;
		case BOIDS_DEMO.second:
			for (auto& boid : boid_visuals)
			{
				boid.Draw();
			}
			container.Draw();
			break;
		default:
			break;
		}
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
