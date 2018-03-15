// Local Headers
#include "glitter.hpp"
#include "lightpts.hpp"
#include "lightpoly.hpp"


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

Camera camera(glm::vec3(0.f, 0.f, 2.f));

int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	// Set callback functions
	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetCursorPosCallback(mWindow, mouse_callback);
	glfwSetScrollCallback(mWindow, scroll_callback);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

	// Create a sample shader that displays normal
	Shader sampleShader(FileSystem::getPath("Shaders/geometry.vert.glsl").c_str(), FileSystem::getPath("Shaders/geometry.frag.glsl").c_str());

	// Load a model from obj file
	//Model sampleModel(FileSystem::getPath("Resources/crytek_sponza/sponza.obj").c_str());

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		glfwPollEvents();
        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)mWidth / (GLfloat)mHeight, mNear, mFar);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4();
		sampleShader.Use();
		model = glm::translate(model, glm::vec3(-1, 0, 0));
		glUniformMatrix4fv(glGetUniformLocation(sampleShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(sampleShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(sampleShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		RenderLightCube(projection, view, model, camera.Position, camera.Front);

		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.05f));    // The sponza model is too big, scale it first
		//glUniformMatrix4fv(glGetUniformLocation(sampleShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//sampleModel.Draw(sampleShader);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
    }   glfwTerminate();
    return EXIT_SUCCESS;
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Camera movements
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camera.ProcessKeyboard(FORWARD, 0.1);
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camera.ProcessKeyboard(BACKWARD, 0.1);
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camera.ProcessKeyboard(LEFT, 0.1);
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camera.ProcessKeyboard(RIGHT, 0.1);
}

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	if (state == GLFW_RELEASE) {
		firstMouse = true;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}