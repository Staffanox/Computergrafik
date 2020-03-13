#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <model.h>
#include <camera.h>
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <irrKlang.h>
#include <filesystem>

using namespace irrklang;

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// error starting up the engine
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const char* musicSrc = "..\\Dependencies\\audio\\25_A_Tavern_on_the_Riverbank.mp3";

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, -1.0f, 2.0f));
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
float move_unit = 0.3f;
glm::vec3 cubePos = glm::vec3(-0.5599f, -0.00f, -0.1227f);
glm::vec3 cubeSize = glm::vec3(0.22396f, 0.00f, 0.0f);
glm::vec3 goalPos = glm::vec3(0.633678f, 0.05703f, -0.1227f);


glm::vec3 powerUpPositions[]{
			glm::vec3(-0.86f, 0.429f, -0.15f),
			glm::vec3(0.13f, -0.0f, -0.15f)

};

glm::vec3 powerUpSize[]{
	glm::vec3(0.172f,0.0858f,0.0f),
	glm::vec3(0.026f,0.0f,0.0f)

};


bool collisionPowerUp(glm::vec3 collisionObject[], glm::vec3 collisionSize[], unsigned int key) {
	std::cout << sizeof(collisionObject) / sizeof(*collisionObject) << std::endl;
	std::cout << key << std::endl;
	for (unsigned int i = 0; i < sizeof(collisionObject) / sizeof(*collisionObject); i++) {
		std::cout << i << std::endl;

		switch (key) {
		case 265:
			if (cubePos.y + cubeSize.y >= collisionObject[i].y && collisionObject[i].y + collisionSize[i].y) {
				std::cout << cubePos.y << std::endl;
				std::cout << cubeSize.y << std::endl;
				std::cout << collisionObject[i].y << std::endl;
				std::cout << collisionSize[i].y << std::endl;

				return true;
				
			}
			break;
		case 264:
			if (cubePos.y + cubeSize.y >= (collisionObject[i].y) && (collisionObject[i].y) + (collisionSize[i].y))
				return true;
			break;
		case 263:
			if (cubePos.x + cubeSize.x >= collisionObject[i].x && collisionObject[i].x + collisionSize[i].x)
				return true;
			break;
		case 262:
			if (cubePos.x + cubeSize.x >= collisionObject[i].x && collisionObject[i].x + collisionSize[i].x)
				return true;
			break;

		default:
			std::cout << "hi" << std::endl;
			return false;



		}


	}
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// start the sound engine with default parameters
	//ISoundEngine* engine = createIrrKlangDevice();
	//if (!engine)
	//	return 100; // error starting up the engine
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
	Shader lampShader("lamp.vs", "lamp.fs");
	Shader myCubeShader("cube.vs", "cube.fs");
	Shader myBoardShader("vertex.vs", "fragment.fs");
	Shader powerUpShader("powerUp.vs", "powerUp.fs");

	//model loading
	Model myCubeModel("..\\Computergrafik\\models\\cube\\cube_fbx.fbx");
	Model myBoardModel("..\\Computergrafik\\models\\board\\board.obj");



	glm::vec3 cubePointLightPositions[] = {
			glm::vec3(0.0f, -1.1f, 0.0f),
			glm::vec3(0.0f, -1.1f, 0.0f),
			glm::vec3(0.0f,-1.1f,-0.2f),
			glm::vec3(0.0f, -1.1f, 0.0f),
			glm::vec3(0.0f, -1.1f, 0.0f),
			glm::vec3(0.0f,0.0f,-0.05f),
			glm::vec3(0.633678f, 0.05703f, 0.1227)

	};

	glm::vec3 boardPointLightPositions[] = {
			glm::vec3(-0.35f, -0.425f, -0.15f),
			glm::vec3(-0.86f, 0.429f, -0.15f),
			glm::vec3(0.43f, -0.44f, -0.15f),
			glm::vec3(0.13f, -0.0f, -0.15f),
			glm::vec3(0.6f, 0.44f, -0.15f),
			glm::vec3(0.84f, -0.25f, -0.15f),
			glm::vec3(0.85f, -0.44f, -0.15f),
			glm::vec3(-0.83f, -0.194f, -0.15f),


	};

	


	glm::vec3 powerUpLightPositions[]{
			glm::vec3(-0.86f, 0.429f, -0.05f),
			glm::vec3(0.13f, -0.0f, -0.05f)


	};


	// load and create a texture 
	// -------------------------
	unsigned int texture1;
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
	myBoardShader.setInt("texture1", 0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	//Play background music during game
	//engine->setSoundVolume(0.01f);
	//engine->play2D(musicSrc, GL_TRUE);


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
		myBoardShader.use();


		// directional light
		glm::vec3 boardDirectLightColor = glm::vec3(1.0f, 1.0f, 1.0f);

		//direct Light intensity
		glm::vec3 boardDiffuseColor = boardDirectLightColor * glm::vec3(0.1f);
		glm::vec3 boardAmbientColor = boardDiffuseColor * glm::vec3(0.25f);
		glm::vec3 boardSpecularColor = glm::vec3(0.00f);


		//Point light intensity
		glm::vec3 boardLightColor;
		boardLightColor.x = cos(glfwGetTime() * 1.7f);
		boardLightColor.y = sin(glfwGetTime() * 2.5f);
		boardLightColor.z = cos(glfwGetTime() * 1.5f);

		glm::vec3 boardDiffuseColorPoint = glm::vec3(0.6f);
		glm::vec3 boardAmbientColorPoint = glm::vec3(0.2f);

		glm::vec3 pulsatingGreenDiffuse = boardLightColor * glm::vec3(0.5f);
		glm::vec3 pulsatingGreenAmbient = pulsatingGreenDiffuse * glm::vec3(0.0f);







		myBoardShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);

		myBoardShader.setVec3("dirLight.direction", -0.0f, 0.25f, -3.5f);
		myBoardShader.setVec3("dirLight.ambient", boardAmbientColor);
		myBoardShader.setVec3("dirLight.diffuse", boardDiffuseColor);
		myBoardShader.setVec3("dirLight.specular", boardAmbientColor);

		// board material
		myBoardShader.setVec3("material.ambient", 0.25f, 0.25f, 0.25f);
		myBoardShader.setVec3("material.diffuse", 0.4f, 0.4f, 0.4f);
		myBoardShader.setVec3("material.specular", 0.774597f, 0.774597f, 0.774597f);
		myBoardShader.setFloat("material.shininess", 76.8f);

		myBoardShader.setVec3("light.ambient", boardAmbientColor);
		myBoardShader.setVec3("light.diffuse", boardDiffuseColor);
		myBoardShader.setVec3("pointLights[0].position", boardPointLightPositions[0]);
		myBoardShader.setVec3("pointLights[0].ambient", pulsatingGreenAmbient);
		myBoardShader.setVec3("pointLights[0].diffuse", pulsatingGreenDiffuse);
		myBoardShader.setVec3("pointLights[0].specular", boardSpecularColor);
		myBoardShader.setFloat("pointLights[0].constant", 1.0f);
		myBoardShader.setFloat("pointLights[0].linear", 0.7f);
		myBoardShader.setFloat("pointLights[0].quadratic", 1.8f);

		myBoardShader.setVec3("pointLights[1].position", boardPointLightPositions[1]);
		myBoardShader.setVec3("pointLights[1].ambient", pulsatingGreenAmbient);
		myBoardShader.setVec3("pointLights[1].diffuse", pulsatingGreenDiffuse);
		myBoardShader.setVec3("pointLights[1].specular", boardSpecularColor);
		myBoardShader.setFloat("pointLights[1].constant", 1.0f);
		myBoardShader.setFloat("pointLights[1].linear", 0.7f);
		myBoardShader.setFloat("pointLights[1].quadratic", 1.8f);

		myBoardShader.setVec3("pointLights[2].position", boardPointLightPositions[2]);
		myBoardShader.setVec3("pointLights[2].ambient", pulsatingGreenAmbient);
		myBoardShader.setVec3("pointLights[2].diffuse", pulsatingGreenDiffuse);
		myBoardShader.setVec3("pointLights[2].specular", boardSpecularColor);
		myBoardShader.setFloat("pointLights[2].constant", 1.0f);
		myBoardShader.setFloat("pointLights[2].linear", 0.7f);
		myBoardShader.setFloat("pointLights[2].quadratic", 1.8f);

		myBoardShader.setVec3("pointLights[3].position", boardPointLightPositions[3]);
		myBoardShader.setVec3("pointLights[3].ambient", pulsatingGreenAmbient);
		myBoardShader.setVec3("pointLights[3].diffuse", pulsatingGreenDiffuse);
		myBoardShader.setVec3("pointLights[3].specular", boardSpecularColor);
		myBoardShader.setFloat("pointLights[3].constant", 1.0f);
		myBoardShader.setFloat("pointLights[3].linear", 0.7f);
		myBoardShader.setFloat("pointLights[3].quadratic", 1.8f);

		myBoardShader.setVec3("pointLights[4].position", boardPointLightPositions[4]);
		myBoardShader.setVec3("pointLights[4].ambient", pulsatingGreenAmbient);
		myBoardShader.setVec3("pointLights[4].diffuse", pulsatingGreenDiffuse);
		myBoardShader.setVec3("pointLights[4].specular", boardSpecularColor);
		myBoardShader.setFloat("pointLights[4].constant", 1.0f);
		myBoardShader.setFloat("pointLights[4].linear", 0.7f);
		myBoardShader.setFloat("pointLights[4].quadratic", 1.8f);

		myBoardShader.setVec3("pointLights[5].position", boardPointLightPositions[5]);
		myBoardShader.setVec3("pointLights[5].ambient", pulsatingGreenAmbient);
		myBoardShader.setVec3("pointLights[5].diffuse", pulsatingGreenDiffuse);
		myBoardShader.setVec3("pointLights[5].specular", boardSpecularColor);
		myBoardShader.setFloat("pointLights[5].constant", 1.0f);
		myBoardShader.setFloat("pointLights[5].linear", 0.7f);
		myBoardShader.setFloat("pointLights[5].quadratic", 1.8f);

		myBoardShader.setVec3("pointLights[6].position", boardPointLightPositions[6]);
		myBoardShader.setVec3("pointLights[6].ambient", pulsatingGreenAmbient);
		myBoardShader.setVec3("pointLights[6].diffuse", pulsatingGreenDiffuse);
		myBoardShader.setVec3("pointLights[6].specular", boardSpecularColor);
		myBoardShader.setFloat("pointLights[6].constant", 1.0f);
		myBoardShader.setFloat("pointLights[6].linear", 0.7f);
		myBoardShader.setFloat("pointLights[6].quadratic", 1.8f);

		myBoardShader.setVec3("pointLights[7].position", boardPointLightPositions[7]);
		myBoardShader.setVec3("pointLights[7].ambient", pulsatingGreenAmbient);
		myBoardShader.setVec3("pointLights[7].diffuse", pulsatingGreenDiffuse);
		myBoardShader.setVec3("pointLights[7].specular", boardSpecularColor);
		myBoardShader.setFloat("pointLights[7].constant", 1.0f);
		myBoardShader.setFloat("pointLights[7].linear", 0.7f);
		myBoardShader.setFloat("pointLights[7].quadratic", 1.8f);



		// pass projection matrix to shader (note that in this case it could change every frame)

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		myBoardShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		myBoardShader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 0.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));

		myBoardShader.setMat4("model", model);
		myBoardModel.Draw(myBoardShader);


		//cube
		//================================


		//changing lightpos over time
		cubePointLightPositions[0].x = cubePos.x - ((sin(glfwGetTime()) / 2.0f));

		cubePointLightPositions[0].y = (cubePos.y);
		cubePointLightPositions[0].z = 0.2f - abs(sin(glfwGetTime()) / 2.0f);

		cubePointLightPositions[1].x = cubePos.x;
		cubePointLightPositions[1].y = cubePos.y - ((sin(glfwGetTime()) / 2.0f));
		cubePointLightPositions[1].z = cubePointLightPositions[0].z;

		cubePointLightPositions[2].x = cubePos.x;
		cubePointLightPositions[2].y = cubePos.y;

		cubePointLightPositions[3] = cubePointLightPositions[0];
		cubePointLightPositions[3].x = cubePos.x + ((sin(glfwGetTime()) / 2.0f));

		cubePointLightPositions[4] = cubePointLightPositions[1];
		cubePointLightPositions[4].y = cubePos.y + ((sin(glfwGetTime()) / 2.0f));

		cubePointLightPositions[5].x = cubePointLightPositions[2].x;
		cubePointLightPositions[5].y = cubePointLightPositions[2].y;





		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);


		glm::vec3 diffuseColor = lightColor * glm::vec3(0.15f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.25f);
		glm::vec3 diffuseColorTopDown = lightColor * glm::vec3(0.15f);
		glm::vec3 ambientColorTopDown = diffuseColorTopDown * glm::vec3(0.35f);
		glm::vec3 specularColor = glm::vec3(0.05f);
		myCubeShader.use();

		myCubeShader.setVec3("viewPos", camera.Position);

		myCubeShader.setVec3("light.ambient", ambientColor);
		myCubeShader.setVec3("light.diffuse", diffuseColor);
		myCubeShader.setVec3("pointLights[0].position", cubePointLightPositions[0]);
		myCubeShader.setVec3("pointLights[0].ambient", ambientColor);
		myCubeShader.setVec3("pointLights[0].diffuse", diffuseColor);
		myCubeShader.setVec3("pointLights[0].specular", specularColor);
		myCubeShader.setFloat("pointLights[0].constant", 1.0f);
		myCubeShader.setFloat("pointLights[0].linear", 0.7f);
		myCubeShader.setFloat("pointLights[0].quadratic", 1.8f);
		// point light 2
		myCubeShader.setVec3("pointLights[1].position", cubePointLightPositions[1]);
		myCubeShader.setVec3("pointLights[1].ambient", ambientColor);
		myCubeShader.setVec3("pointLights[1].diffuse", diffuseColor);
		myCubeShader.setVec3("pointLights[1].specular", specularColor);
		myCubeShader.setFloat("pointLights[1].constant", 1.0f);
		myCubeShader.setFloat("pointLights[1].linear", 0.7f);
		myCubeShader.setFloat("pointLights[1].quadratic", 1.8f);

		// point light 3
		myCubeShader.setVec3("pointLights[2].position", cubePointLightPositions[2]);
		myCubeShader.setVec3("pointLights[2].ambient", ambientColor);
		myCubeShader.setVec3("pointLights[2].diffuse", diffuseColorTopDown);
		myCubeShader.setVec3("pointLights[2].specular", 0.5f, 0.5f, 0.5f);
		myCubeShader.setFloat("pointLights[2].constant", 1.0f);
		myCubeShader.setFloat("pointLights[2].linear", 0.7f);
		myCubeShader.setFloat("pointLights[2].quadratic", 1.8f);

		myCubeShader.setVec3("pointLights[3].position", cubePointLightPositions[3]);
		myCubeShader.setVec3("pointLights[3].ambient", ambientColor);
		myCubeShader.setVec3("pointLights[3].diffuse", diffuseColor);
		myCubeShader.setVec3("pointLights[3].specular", specularColor);
		myCubeShader.setFloat("pointLights[3].constant", 1.0f);
		myCubeShader.setFloat("pointLights[3].linear", 0.7f);
		myCubeShader.setFloat("pointLights[3].quadratic", 1.8f);

		myCubeShader.setVec3("pointLights[4].position", cubePointLightPositions[4]);
		myCubeShader.setVec3("pointLights[4].ambient", ambientColor);
		myCubeShader.setVec3("pointLights[4].diffuse", diffuseColor);
		myCubeShader.setVec3("pointLights[4].specular", specularColor);
		myCubeShader.setFloat("pointLights[4].constant", 1.0f);
		myCubeShader.setFloat("pointLights[4].linear", 0.7f);
		myCubeShader.setFloat("pointLights[4].quadratic", 1.8f);

		myCubeShader.setVec3("pointLights[5].position", cubePointLightPositions[5]);
		myCubeShader.setVec3("pointLights[5].ambient", ambientColorTopDown);
		myCubeShader.setVec3("pointLights[5].diffuse", diffuseColorTopDown);
		myCubeShader.setVec3("pointLights[5].specular", specularColor);
		myCubeShader.setFloat("pointLights[5].constant", 1.0f);
		myCubeShader.setFloat("pointLights[5].linear", 0.7f);
		myCubeShader.setFloat("pointLights[5].quadratic", 1.8f);

		myCubeShader.setVec3("pointLights[6].position", cubePointLightPositions[6]);
		myCubeShader.setVec3("pointLights[6].ambient", glm::vec3(0.5f));
		myCubeShader.setVec3("pointLights[6].diffuse", glm::vec3(0.25f));
		myCubeShader.setVec3("pointLights[6].specular", specularColor);
		myCubeShader.setFloat("pointLights[6].constant", 1.0f);
		myCubeShader.setFloat("pointLights[6].linear", 0.7f);
		myCubeShader.setFloat("pointLights[6].quadratic", 1.8f);


		// material properties
		myCubeShader.setVec3("material.ambient", 0.25f, 0.20725f, 0.20725f);
		myCubeShader.setVec3("material.diffuse", 1.0f, 0.829f, 0.829f);
		myCubeShader.setVec3("material.specular", 0.296648f, 0.296648f, 0.296648f); // specular lighting doesn't have full effect on this object's material
		myCubeShader.setFloat("material.shininess", 11.264f);

		myCubeShader.setMat4("projection", projection);
		myCubeShader.setMat4("view", view);
		myCubeShader.setVec3("viewPos", camera.Position);

		//render
		glm::mat4 myCubeModelMat = glm::mat4(1.0f);
		myCubeModelMat = glm::translate(myCubeModelMat, glm::vec3(cubePos));
		myCubeModelMat = glm::scale(myCubeModelMat, glm::vec3(0.04f, 0.04f, 0.04f));
		myCubeShader.setMat4("model", myCubeModelMat);
		myCubeModel.Draw(myCubeShader);

		//cube end
		//================================

		//start goal
		glm::mat4 goalMat = glm::mat4(1.0f);
		goalMat = glm::translate(goalMat, glm::vec3(goalPos));
		goalMat = glm::scale(goalMat, glm::vec3(0.04f, 0.04f, 0.0004f));
		myCubeShader.setMat4("model", goalMat);
		myCubeModel.Draw(myCubeShader);

		//end goal


		//start powerups



		glm::vec3 powerUpDiffuse = glm::vec3(0.5f);
		glm::vec3 powerUpAmbient = glm::vec3(0.1f);
		glm::vec3 powerUpSpecular = glm::vec3(0.05f);



		powerUpShader.setVec3("light.ambient", powerUpAmbient);
		powerUpShader.setVec3("light.diffuse", powerUpDiffuse);
		powerUpShader.setVec3("pointLights[0].position", powerUpLightPositions[0]);
		powerUpShader.setVec3("pointLights[0].ambient", powerUpAmbient);
		powerUpShader.setVec3("pointLights[0].diffuse", powerUpDiffuse);
		powerUpShader.setVec3("pointLights[0].specular", boardSpecularColor);
		powerUpShader.setFloat("pointLights[0].constant", 1.0f);
		powerUpShader.setFloat("pointLights[0].linear", 0.7f);
		powerUpShader.setFloat("pointLights[0].quadratic", 1.8f);

		powerUpShader.setVec3("pointLights[1].position", powerUpLightPositions[1]);
		powerUpShader.setVec3("pointLights[1].ambient", powerUpAmbient);
		powerUpShader.setVec3("pointLights[1].diffuse", powerUpDiffuse);
		powerUpShader.setVec3("pointLights[1].specular", boardSpecularColor);
		powerUpShader.setFloat("pointLights[1].constant", 1.0f);
		powerUpShader.setFloat("pointLights[1].linear", 0.7f);
		powerUpShader.setFloat("pointLights[1].quadratic", 1.8f);

		for (unsigned int i = 0; i < sizeof(powerUpPositions)/sizeof(*powerUpPositions);i++) {
		glm::mat4 powerUpMat = glm::mat4(1.0f);
		powerUpMat = glm::translate(powerUpMat, glm::vec3(powerUpPositions[i]));
		powerUpMat = glm::scale(powerUpMat, glm::vec3(0.02f, 0.02f, 0.02f));
		powerUpShader.setMat4("model", powerUpMat);
		myCubeModel.Draw(powerUpShader);
		
	}




		// also draw the lamp object(s)
		/*lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightVAO);

		 for (unsigned int i = 0; i < 2; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, powerUpLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.02f)); // Make it a smaller cube
			lampShader.setMat4("model", model);
			myCubeModel.Draw(lampShader);
		}

		 */


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	// not needed. will be done in model.h as well as the drawing

	//engine->drop();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input query GLFW whether relevant keys are pressed/released this frame and react accordingly
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
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) 
		camera.Position = glm::vec3(cubePos.x, cubePos.y, camera.Position.z);
		
	
	//move cube
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		cubePos.y += move_unit * deltaTime;
		if (collisionPowerUp(powerUpPositions, powerUpSize,(int)GLFW_KEY_UP))
			move_unit = move_unit * 1.5f;
		//std::cout << move_unit << std::endl;
		//move up
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		cubePos.y -= move_unit * deltaTime;
		if (collisionPowerUp(powerUpPositions, powerUpSize,(int)GLFW_KEY_DOWN))
			move_unit = move_unit * 1.5f;		//move down
		std::cout << move_unit << std::endl;


	}


	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		cubePos.x -= move_unit * deltaTime;
		if (collisionPowerUp(powerUpPositions, powerUpSize,(int)GLFW_KEY_LEFT))
			move_unit = move_unit * 1.5f;
		std::cout << move_unit << std::endl;

		//move left

	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		cubePos.x += move_unit * deltaTime;
		if (collisionPowerUp(powerUpPositions,powerUpSize, (int)GLFW_KEY_RIGHT))
			move_unit = move_unit *1.5f;
		std::cout << move_unit << std::endl;

		//move right

	}

	//speed button for cube
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		//std::cout << currentSprintTime << std::endl;
		if (sprintPressed == false) {
			currentSprintTime = (float)glfwGetTime();
			sprintPressed = true;
		}
		sprintTime = (float)glfwGetTime();

		//speed during sprint
		if (currentSprintTime + 5.0f >= sprintTime) {
			move_unit = 0.5f;
		}
		//speed after sprint (fatigue)
		else {
			move_unit = 0.01f;
		}
		//std::cout << move_unit << std::endl;
	}
	//reset to normal speed after release
	if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) && (sprintPressed == true)) {
		//speed after sprint (recovery)
		if (((float)glfwGetTime() >= sprintTime + 1.0f)) {
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

