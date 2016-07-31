#include <iostream>
#include <memory>

// GLEW
//#define GLEW_STATIC
#include <glew/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL/SOIL.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
// Other includes
#include "Shader.h"
#include "camera.h"
#include "model.h"
#include "Player.h"
#include "TextRender.h"

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xpos, double ypos);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

//mode 
enum Mode {
	FIRST_PERSPECTIVE,
	THIRD_PERSPECTIVE
};

//camera
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
shared_ptr<Camera> current_camera;
shared_ptr<Player> player_ptr;
Mode view_mode = THIRD_PERSPECTIVE;

//project_path
string project_path = "D:\\Projects\\cs\\cs\\";

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "cs", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Compile and setup the shader
	Shader model_shader("model.vs", "model.frag");
	Shader text_shader("text.vs", "text.frag");
	
	TextRender text_render;
	text_render.load_font("arial.ttf");

	//load scene model and player model
	Model scene_model(project_path + "Sand_Final.obj");
	shared_ptr<Player> player(new Player(project_path + "cube.obj"));
	player_ptr = player;
	if (view_mode == FIRST_PERSPECTIVE)
		current_camera = static_pointer_cast<Camera>(player);
	else
		current_camera = make_shared<Camera>(player_ptr->get_position());

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Render
		// Clear the color buffer
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//// Activate shader
		model_shader.Use();

		// Create transformations
		glm::mat4 view;
		view = current_camera->GetView();
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		glm::mat4 projection;
		projection = glm::perspective(current_camera->GetAspect(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get their uniform location
		GLint viewLoc = glGetUniformLocation(model_shader.Program, "view");
		GLint projLoc = glGetUniformLocation(model_shader.Program, "projection");
		// Pass them to the shaders
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, vec3(0,-1.5,0)); // Translate it down a bit so it's at the center of the scene
		glUniformMatrix4fv(glGetUniformLocation(model_shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		scene_model.Draw(model_shader);

		//draw player
		if (view_mode == FIRST_PERSPECTIVE)
		{
			glm::mat4 model;
			model = glm::translate(model, player->get_position() + vec3(10, 0, 0)); // Translate it down a bit so it's at the center of the scene
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// It's a bit too big for our scene, so scale it down
			glUniformMatrix4fv(glGetUniformLocation(model_shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			player->draw(model_shader);
		}

		//render text 
		text_shader.Use();
		projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
		glUniformMatrix4fv(glGetUniformLocation(text_shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		text_render.draw(text_shader, "hello world", 25.0f, 25.0f, 1, vec3(0.5f, 0.8f, 0.2f));
		
		// Swap the screen buffers
		glfwSwapBuffers(window);

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (view_mode == FIRST_PERSPECTIVE)
		{
			view_mode = THIRD_PERSPECTIVE;
			current_camera = make_shared<Camera>(player_ptr->get_position());
		}
		else
		{
			view_mode = FIRST_PERSPECTIVE;
			current_camera = static_pointer_cast<Camera>(player_ptr);
		}
	}
	if (action == GLFW_PRESS)
		keys[key] = true;
	if (action == GLFW_RELEASE)
		keys[key] = false;
}

void do_movement()
{
	if (keys[GLFW_KEY_W])
		current_camera->KeyMove(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		current_camera->KeyMove(BACKWORD, deltaTime);
	if (keys[GLFW_KEY_A])
		current_camera->KeyMove(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		current_camera->KeyMove(RIGHT, deltaTime);
	if (keys[GLFW_KEY_SPACE])
		current_camera->KeyMove(JUMP, deltaTime);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	current_camera->MouseMove(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xpos, double ypos)
{
	current_camera->Zoom(ypos);
}