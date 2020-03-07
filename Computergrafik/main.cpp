#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <camera.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float sprintTime = 0.0;
float currentSprintTime = 0.0;
bool sprintPressed = false;
//cube
float move_unit = 0.1f;
float cube_posX = 0, cube_posY = 0, cube_posZ = 0;

// lighting
glm::vec3 lightPos(0.0f, 0.5f, 0.0f);

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("vertex.vs", "fragment.fs");
	Shader cubeShader("cube.vs", "cube.fs");
	Shader lampShader("lamp.vs", "lamp.fs");


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float board[] = {
		-1.0f, -0.2f, -0.5f,  0.0f, 0.0f,
		 1.0f, -0.2f, -0.5f,  1.0f, 0.0f,
		 1.0f,  0.2f, -0.5f,  1.0f, 1.0f,
		 1.0f,  0.2f, -0.5f,  1.0f, 1.0f,
		-1.0f,  0.2f, -0.5f,  0.0f, 1.0f,
		-1.0f, -0.2f, -0.5f,  0.0f, 0.0f,

		-1.0f, -0.2f,  0.5f,  0.0f, 0.0f,
		 1.0f, -0.2f,  0.5f,  1.0f, 0.0f,
		 1.0f,  0.2f,  0.5f,  1.0f, 1.0f,
		 1.0f,  0.2f,  0.5f,  1.0f, 1.0f,
		-1.0f,  0.2f,  0.5f,  0.0f, 1.0f,
		-1.0f, -0.2f,  0.5f,  0.0f, 0.0f,

		-1.0f,  0.2f,  0.5f,  1.0f, 0.0f,
		-1.0f,  0.2f, -0.5f,  1.0f, 1.0f,
		-1.0f, -0.2f, -0.5f,  0.0f, 1.0f,
		-1.0f, -0.2f, -0.5f,  0.0f, 1.0f,
		-1.0f, -0.2f,  0.5f,  0.0f, 0.0f,
		-1.0f,  0.2f,  0.5f,  1.0f, 0.0f,

		 1.0f,  0.2f,  0.5f,  1.0f, 0.0f,
		 1.0f,  0.2f, -0.5f,  1.0f, 1.0f,
		 1.0f, -0.2f, -0.5f,  0.0f, 1.0f,
		 1.0f, -0.2f, -0.5f,  0.0f, 1.0f,
		 1.0f, -0.2f,  0.5f,  0.0f, 0.0f,
		 1.0f,  0.2f,  0.5f,  1.0f, 0.0f,

		-1.0f, -0.2f, -0.5f,  0.0f, 1.0f,
		 1.0f, -0.2f, -0.5f,  1.0f, 1.0f,
		 1.0f, -0.2f,  0.5f,  1.0f, 0.0f,
		 1.0f, -0.2f,  0.5f,  1.0f, 0.0f,
		-1.0f, -0.2f,  0.5f,  0.0f, 0.0f,
		-1.0f, -0.2f, -0.5f,  0.0f, 1.0f,

		-1.0f,  0.2f, -0.5f,  0.0f, 1.0f,
		 1.0f,  0.2f, -0.5f,  1.0f, 1.0f,
		 1.0f,  0.2f,  0.5f,  1.0f, 0.0f,
		 1.0f,  0.2f,  0.5f,  1.0f, 0.0f,
		-1.0f,  0.2f,  0.5f,  0.0f, 0.0f,
		-1.0f,  0.2f, -0.5f,  0.0f, 1.0f
	};

	float cube[] = {
		//right side
	   -0.03f,  0.22f, -0.03f, 0.0f, 0.0f,-1.0f,
		0.03f,  0.22f, -0.03f, 0.0f, 0.0f,-1.0f,
		0.03f,  0.3f, -0.03f,  0.0f, 0.0f,-1.0f,
		0.03f,  0.3f, -0.03f,  0.0f, 0.0f,-1.0f,
	   -0.03f,  0.3f, -0.03f,  0.0f, 0.0f,-1.0f,
	   -0.03f,  0.22f, -0.03f, 0.0f, 0.0f,-1.0f,

	   //left side
	   -0.03f,  0.22f,  0.03f, 0.0f, 0.0f,1.0f,
		0.03f,  0.22f,  0.03f, 0.0f, 0.0f,1.0f,
		0.03f,  0.3f,  0.03f,  0.0f, 0.0f,1.0f,
		0.03f,  0.3f,  0.03f,  0.0f, 0.0f,1.0f,
	   -0.03f,  0.3f,  0.03f,  0.0f, 0.0f,1.0f,
	   -0.03f,  0.22f,  0.03f, 0.0f, 0.0f,1.0f,

	   //front
	   -0.03f,  0.3f,  0.03f,  0.0f, 0.0f,0.0f,
	   -0.03f,  0.3f, -0.03f,  0.0f, 0.0f,0.0f,
	   -0.03f,  0.22f, -0.03f, 0.0f, 0.0f,0.0f,
	   -0.03f,  0.22f, -0.03f, 0.0f, 0.0f,0.0f,
	   -0.03f,  0.22f,  0.03f, 0.0f, 0.0f,0.0f,
	   -0.03f,  0.3f,  0.03f,  0.0f, 0.0f,0.0f,

	   //back
		0.03f,  0.3f,  0.03f,  0.0f, 0.0f,0.0f,
		0.03f,  0.3f, -0.03f,  0.0f, 0.0f,0.0f,
		0.03f,  0.22f, -0.03f, 0.0f, 0.0f,0.0f,
		0.03f,  0.22f, -0.03f, 0.0f, 0.0f,0.0f,
		0.03f,  0.22f,  0.03f, 0.0f, 0.0f,0.0f,
		0.03f,  0.3f,  0.03f,  0.0f, 0.0f,0.0f,

		//bottom
	   -0.03f,  0.22f, -0.03f,  1.0f, 1.0f,1.0f,
		0.03f,  0.22f, -0.03f,  1.0f, 1.0f,1.0f,
		0.03f,  0.22f,  0.03f,  1.0f, 1.0f,1.0f,
		0.03f,  0.22f,  0.03f,  1.0f, 1.0f,1.0f,
	   -0.03f,  0.22f,  0.03f,  1.0f, 1.0f,1.0f,
	   -0.03f,  0.22f, -0.03f,  1.0f, 1.0f,1.0f,

	   //top
	   -0.03f,  0.3f, -0.03f,  1.0f, 1.0f,1.0f,
		0.03f,  0.3f, -0.03f,  1.0f, 1.0f,1.0f,
		0.03f,  0.3f,  0.03f,  1.0f, 1.0f,1.0f,
		0.03f,  0.3f,  0.03f,  1.0f, 1.0f,1.0f,
	   -0.03f,  0.3f,  0.03f,  1.0f, 1.0f,1.0f,
	   -0.03f,  0.3f, -0.03f,  1.0f, 1.0f,1.0f,
	};


	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(board), board, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// load and create a texture 
	// -------------------------
	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(("..\\Dependencies\\resources\\container.jpg"), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture1", 0);




	unsigned int cubeVBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		//board
		// activate shader
		ourShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		glBindVertexArray(VAO);

		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//changing lightpos over time
		lightPos.x =  cube_posX- ((sin(glfwGetTime()) / 3.0f));
		lightPos.y = 0.5f - abs(sin(glfwGetTime()) / 3);
		lightPos.z = cube_posZ -((sin(glfwGetTime())/ 3.0f));


		std::cout << lightPos.y << std::endl;
		//std::cout << lightPos.z << std::endl;
		//std::cout << lightPos.z << std::endl;


		//cube
		// be sure to activate shader when setting uniforms/drawing objects
		cubeShader.use();
		cubeShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		cubeShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		cubeShader.setVec3("lightPos", lightPos);
		cubeShader.setVec3("viewPos", camera.Position);

		// view/projection transformations
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("view", view);

		// world transformation
		glm::mat4 cube_model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		cube_model = glm::translate(cube_model, glm::vec3(cube_posX, cube_posY, cube_posZ));
		cubeShader.setMat4("model", cube_model);

		// render the cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		lampShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//move cube
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		cube_posZ -= move_unit*deltaTime;
		//move up
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		cube_posZ += move_unit * deltaTime;
		//move down
	}


	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		cube_posX -= move_unit * deltaTime;
		//move left
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		cube_posX += move_unit * deltaTime;
		//move right
	}

	//speed button for cube
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		//std::cout << currentSprintTime << std::endl;
		if (sprintPressed == false) {
			currentSprintTime = (float) glfwGetTime();
			sprintPressed = true;
		}
		sprintTime = (float)glfwGetTime();

		//speed during sprint
		if (currentSprintTime+5.0f >= sprintTime) {
			move_unit = 0.5f;
		}
		//speed after sprint (fatigue)
		else {
			move_unit = 0.01f;
		}
		//std::cout << move_unit << std::endl;
	}
	//reset to normal speed after release
	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)&& (sprintPressed == true)) {
		//speed after sprint (recovery)
		if (((float)glfwGetTime() >= sprintTime + 5.0f)) {
			move_unit = 0.05f;
			//std::cout << move_unit << std::endl;

			if ((float)glfwGetTime() >= sprintTime + 6.5f) {
				sprintPressed = false;
				move_unit = 0.1f;
				sprintTime = 0.0f;
				currentSprintTime = 0.0f;
				//std::cout << move_unit << std::endl;

			}
		}
		else {
			move_unit = 0.1f;
			//std::cout << move_unit << std::endl;

		}
		
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}