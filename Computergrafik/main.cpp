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
#include <math.h>
#include <irrKlang.h>
#include <filesystem>

using namespace irrklang;

#define _USE_MATH_DEFINES


#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// error starting up the engine
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
bool collisionCheck(glm::vec3 cubePos, glm::vec3 collisionObject[], glm::vec3 collisionSize[]);
void collisionGoal(glm::vec3 collisionObject[], glm::vec3 collisionSize[]);
double dotProduct(glm::vec3 objA, glm::vec3 objB);

unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

const char* musicSrc = "..\\Dependencies\\audio\\25_A_Tavern_on_the_Riverbank.mp3";

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, -1.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
const glm::vec3 constCamera = glm::vec3(0.0f, -1.0f, 2.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float sprintTime = 0.0;
float currentSprintTime = 0.0;
bool sprintPressed = false;

Model myCubeModel;
Model myBoardModel;

double lowest = 10;

//cube
float move_unit = 0.3f;
glm::vec3 cubePos = glm::vec3(-0.5599f, 0.0f, -0.1227f);
glm::vec3 cubeSize = glm::vec3(0.0004f, 0.04f, 0.04f);
glm::vec3 goalPos[] = { glm::vec3(0.633678f, 0.05703f, -0.1227f) };
glm::vec3 goalSize[] = { glm::vec3(0.04f,0.02f,0.02f) };

glm::vec3 powerUpPositions[]{ glm::vec3(-0.86f, 0.429f, -0.15f), };
glm::vec3 powerUpSize[]{ glm::vec3(0.0004f,0.02f,0.02f), };


bool collisionPowerUp(glm::vec3 collisionObject[], glm::vec3 collisionSize[]) {
	if (collisionCheck(cubePos, collisionObject, collisionSize))
		return true;
	else
		return false;
}


//collision
bool collisionXleft = false;
bool collisionXright = false;
bool collisionYtop = false;
bool collisionYdown = false;

void doBlock(int GLFW_KEY_PRESSED) {

	switch (GLFW_KEY_PRESSED) {

	case(GLFW_KEY_UP):
		collisionYtop = true;
		std::cout << " UP " << endl;
		break;

	case(GLFW_KEY_LEFT):
		collisionXleft = true;
		std::cout << " LEFT " << endl;
		break;

	case(GLFW_KEY_RIGHT):
		collisionXright = true;
		std::cout << " RIGHT " << endl;
		break;

	case(GLFW_KEY_DOWN):
		collisionYdown = true;
		std::cout << " DOWN " << endl;
		break;
	}

}


//normals
bool showNormals = false;

bool cubeIntersection(int GLFW_KEY_PRESSED) {

	/*for (const auto& mesh : myBoardModel.meshes) {
		//std::cout << " new mesh " << endl;
		//std::cout << " size of meshes: " << sizeof(myBoardModel.meshes) << endl;
		//std::cout << " cube: " << cubePos.x << "/" << cubePos.y << endl;

		for (const auto& vertex : mesh.vertices) {

			//std::cout << " x: " << vertex.Position.x << " y: " << vertex.Position.y << endl;
			std::cout << " size of vertices: " << sizeof(mesh.vertices) << endl;

		}
	}*/


	for (const auto& mesh : myBoardModel.meshes) {
		for (const auto& vertex : mesh.vertices) {

			//if (vertex.Position.z > -0.0700f) {

				glm::vec3 cubeFromCam = (cubePos - constCamera);
				glm::vec3 boardFromCam = (vertex.Position - constCamera);

				float angle = atan2(cubeFromCam.y, cubeFromCam.x) - atan2(boardFromCam.y, boardFromCam.x);
				angle = angle / 3.14159265358979323846 * 180;

				if (angle < 0.2 && angle > -0.2) {
					/*std::cout << " cube: " << cubePos.x << "/" << cubePos.y << endl;
					std::cout << " vertex: " << vertex.Position.x << "/" << vertex.Position.y << endl;
					std::cout << " = angle: " << angle << endl;*/

					//Wenn man bspw. nicht weiter in richtung +X moven kann, anschlie�end nach -Y moven m�chte, wird automatisch doBlock ausgef�hrt, da der Winkel immernoch zwischen -0.2 und 0.2 bleibt.
					float difX = cubeFromCam.x - boardFromCam.x;

					if (difX <= 0.3 && difX >= -0.3) {
						if (GLFW_KEY_PRESSED == GLFW_KEY_UP || GLFW_KEY_PRESSED == GLFW_KEY_DOWN) {
							doBlock(GLFW_KEY_PRESSED);
							std::cout << " blocked " << endl;

							return true;
						}
					}

					float difY = cubeFromCam.y - boardFromCam.y;

					if (difY <= 0.3 && difY >= -0.3) {
						if (GLFW_KEY_PRESSED == GLFW_KEY_LEFT || GLFW_KEY_PRESSED == GLFW_KEY_RIGHT) {
							doBlock(GLFW_KEY_PRESSED);
							std::cout << " blocked " << endl;

							return true;
						}
					}
				}
		}
	}
	
	collisionXleft = false;
	collisionXright = false;
	collisionYtop = false;
	collisionYdown = false;
	return false;

}

/*returns false = no collision detected -- returns true = collision detected*/
bool checkdBoardWithCubeCollision(int GLFW_KEY_PRESSED) {
		

	
	collisionXleft = false;
	collisionXright = false;
	collisionYtop = false;
	collisionYdown = false;
	lowest = 10;
	std::cout << "false" << endl;

	return false; //no collision detected
}


//Particles

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Steven einserkandidat", NULL, NULL);
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
	Shader normalShader("normal_visualization.vs", "normal_visualization.fs", "normal_visualization.gs");
	Shader skyboxShader("skybox.vs", "skybox.fs");
	Shader cubeLightingShader("basic_lighting.vs", "basic_lighting.fs");
	Shader lightcastShader("light_casters.vs", "light_casters.fs");


	//load models
	myCubeModel = Model("..\\Computergrafik\\models\\cube\\cube.obj");
	myBoardModel = Model("..\\Computergrafik\\models\\board\\board.obj");

	//SKYBOX

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
// -------------

	vector<std::string> faces
	{
		"..\\Computergrafik\\skybox\\right.png",
		"..\\Computergrafik\\skybox\\left.png",
		"..\\Computergrafik\\skybox\\top.png",
		"..\\Computergrafik\\skybox\\bottom.png",
		"..\\Computergrafik\\skybox\\front.png",
		"..\\Computergrafik\\skybox\\back.png"
	};

	unsigned int cubemapTexture = loadCubemap(faces);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	//SKYBOX END

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

	// shader configuration
	// --------------------
	lightcastShader.use();
	lightcastShader.setInt("material.diffuse", 0);
	lightcastShader.setInt("material.specular", 1);

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

		glm::mat4 boardModelMat = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		boardModelMat = glm::rotate(boardModelMat, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 0.5f));
		boardModelMat = glm::rotate(boardModelMat, glm::radians(90.0f), glm::vec3(0.5f, 0.0f, 0.0f));
		boardModelMat = glm::scale(boardModelMat, glm::vec3(0.1f, 0.1f, 0.1f));
		boardModelMat = glm::translate(boardModelMat, glm::vec3(0.0f, -2.0f, 0.0f));

		myBoardShader.setMat4("model", boardModelMat);
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

		//cube end

		//start goal
		glm::mat4 goalMat = glm::mat4(1.0f);
		goalMat = glm::translate(goalMat, glm::vec3(goalPos[0]));
		goalMat = glm::scale(goalMat, glm::vec3(0.06f, 0.06f, 0.0006f));
		myCubeShader.setMat4("model", goalMat);

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

		glm::mat4 powerUpMat = glm::mat4(1.0f);

		for (unsigned int i = 0; i < sizeof(powerUpPositions)/sizeof(*powerUpPositions);i++) {
		powerUpMat = glm::translate(powerUpMat, glm::vec3(powerUpPositions[i]));
		powerUpMat = glm::scale(powerUpMat, glm::vec3(0.04f, 0.04f, 0.04f));
		powerUpShader.setMat4("model", powerUpMat);
		//myCubeModel.Draw(powerUpShader);
		
	}

		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

		cubeLightingShader.use();
		cubeLightingShader.setVec3("light.position", lightPos);
		cubeLightingShader.setVec3("viewPos", camera.Position);

		// light properties
		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		cubeLightingShader.setVec3("light.ambient", ambientColor);
		cubeLightingShader.setVec3("light.diffuse", diffuseColor);
		cubeLightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		cubeLightingShader.setVec3("material.ambient", 0.24725f, 0.1995f, 0.0745f);
		cubeLightingShader.setVec3("material.diffuse", 0.75164f, 0.60648f, 0.22648f);
		cubeLightingShader.setVec3("material.specular", 0.628281f, 0.555802f, 0.366065f); // specular lighting doesn't have full effect on this object's material
		cubeLightingShader.setFloat("material.shininess", 51.2f);

		cubeLightingShader.setMat4("projection", projection);
		cubeLightingShader.setMat4("view", view);
		
		glm::mat4 movingCubeModel = glm::mat4(1.0f);
		movingCubeModel = glm::translate(movingCubeModel, cubePos);
		movingCubeModel = glm::scale(movingCubeModel, glm::vec3(0.065f, 0.065f, 0.065f));

		cubeLightingShader.setMat4("model", movingCubeModel);
		myCubeModel.Draw(cubeLightingShader);

		/*
		 ----------------------------- CHANGE LIGHTNING FOR POWERUP CUBE HERE -----------------------------
		*/

		cubeLightingShader.setMat4("model", powerUpMat);
		myCubeModel.Draw(cubeLightingShader);

		/*
		----------------------------- CHANGE LIGHTNING FOR GOAL CUBE HERE -----------------------------
		*/

		cubeLightingShader.setMat4("model", goalMat);
		myCubeModel.Draw(cubeLightingShader);

		//draw normals

		if (showNormals) {
			normalShader.use();
			normalShader.setMat4("projection", projection);
			normalShader.setMat4("view", view);
			normalShader.setMat4("model", boardModelMat);

			myBoardModel.Draw(normalShader);


		}
			

		//SKYBOX  -  MUST be last object of this while loop

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		//SKYBOX END

		// also draw the lamp object(s)
		/*lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightVAO);
    
		 for (unsigned int i = 0; i < 8; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, boardPointLightPositions[i]);
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
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);

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
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		showNormals = !showNormals;

		//move cube
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		
			if (!collisionYtop) {
				cubePos.y += move_unit * deltaTime;
				collisionGoal(goalPos, goalSize);
				if (collisionPowerUp(powerUpPositions, powerUpSize))
					move_unit = move_unit * 1.05f;
				//move up
			}
			cubeIntersection(GLFW_KEY_UP);
		}

		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {

			if(!collisionYdown){
				cubePos.y -= move_unit * deltaTime;
				collisionGoal(goalPos, goalSize);
				if (collisionPowerUp(powerUpPositions, powerUpSize))
					move_unit = move_unit * 1.05f;
				//move down
			}
			cubeIntersection(GLFW_KEY_DOWN);
		}


		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			if (!collisionXleft) {
			
			cubePos.x -= move_unit * deltaTime;
			collisionGoal(goalPos, goalSize);
			if (collisionPowerUp(powerUpPositions, powerUpSize))
				move_unit = move_unit * 1.05f;
			//move left
			}
			cubeIntersection(GLFW_KEY_LEFT);
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			if (!collisionXright) {
			cubePos.x += move_unit * deltaTime;
			collisionGoal(goalPos, goalSize);
			if (collisionPowerUp(powerUpPositions, powerUpSize))
				move_unit = move_unit * 1.005f;
			//move right
			}
			cubeIntersection(GLFW_KEY_RIGHT);
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
				move_unit = 0.3f;
			}
		}
		//reset to normal speed after release
		if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) && (sprintPressed == true)) {
			//speed after sprint (recovery)
			if (((float)glfwGetTime() >= sprintTime + 1.0f)) {
				move_unit = 0.05f;

				if ((float)glfwGetTime() >= sprintTime + 6.5f) {
					sprintPressed = false;
					move_unit = 0.1f;
					sprintTime = 0.0f;
					currentSprintTime = 0.0f;

				}
			}
			else {
				move_unit = 0.1f;

			}

		}
	
}


	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	void framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}


	// glfw: whenever the mouse moves, this callback is called
	// -------------------------------------------------------
	void mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
	void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(yoffset);
	}

	bool collisionCheck(glm::vec3 cubePos, glm::vec3 collisionObject[], glm::vec3 collisionSize[]) {
		for (unsigned int i = 0; i < 1; i++) {
			if (cubePos.x - (cubeSize.x / 2) <= (collisionObject[i].x + collisionSize[i].x / 2) && cubePos.x + (cubeSize.x / 2) >= (collisionObject[i].x - collisionSize[i].x / 2) &&
				(cubePos.y - (cubeSize.y / 2) <= (collisionObject[i].y + collisionSize[i].y / 2) && cubePos.y + (cubeSize.y / 2) >= (collisionObject[i].y - collisionSize[i].y / 2)))
				return true;

			else
				return false;
		}

		return false;
	}
void collisionGoal(glm::vec3 collisionObject[], glm::vec3 collisionSize[]) {

	if (collisionCheck(cubePos, collisionObject, collisionSize))
		exit(100);

}



double dotProduct(glm::vec3 objA, glm::vec3 objB) {
	return objA.x * objB.x + objA.y * objB.y + objA.z * objB.z;

}

	// loads a cubemap texture from 6 individual texture faces
	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)
	// -------------------------------------------------------

	// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}