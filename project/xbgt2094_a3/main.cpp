#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <glm/glm.hpp>
#include "camera.h"
#include "shader_utils.h"
#include "mesh_utils.h"
#include "texture_utils.h"
#include "light_utils.h"

void loadShaderProgram(unsigned int* programPtr, const std::string& vertPath, const std::string& fragPath, const std::string& shaderName);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void updateLights();
void drawLightBulbs();

class SimpleBoundingBox
{
public:
	glm::vec3 center;
	glm::vec3 dx, dy, dz;
	glm::vec3 half;

	SimpleBoundingBox(glm::vec3 center, glm::vec3 halfSize) : center(center), half(halfSize) {
		dx = glm::vec3(1, 0, 0);
		dy = glm::vec3(0, 1, 0);
		dz = glm::vec3(0, 0, 1);
	}

	bool isInside(glm::vec3 pt)
	{
		glm::vec3 d = pt - center;
		return abs(dot(d, dx)) <= half.x &&
			abs(dot(d, dy)) <= half.y &&
			abs(dot(d, dz)) <= half.z;;
	}
};

#pragma region Framework Vars

GLFWwindow* window;
const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 768;
unsigned int currentScreenWidth, currentScreenHeight;

// Camera settings
float fov = 60.0f, zNear = 0.1f, zFar = 1000.0f;
Camera camera(fov, zNear, zFar);
glm::vec2 lastCursorPos;

bool appFocused;

#pragma endregion

#pragma region Variables
unsigned int sceneFBO;
unsigned int sceneFBO_colourBuffers[2], sceneFBO_depthBuffer;
unsigned int gBlurFBOs[2];
unsigned int gBlurFBO_colourBuffers[2];
unsigned int buffer_copy;

//unsigned int gauss_buffers[2];
//unsigned int gauss_color_buffers[2];
//unsigned int gaussianBlurShaderProgram;

float appTime;
glm::vec2 mousePosition;

SimpleMesh* skybox, * quad;
SimpleMesh* pool, * pedestal, * flagPole, * flag, * grass, * rocks, * water, * ship;

unsigned int Flag,waterTexture_normal,waterTexture, flagpole_diffuse, flagpole_specular, grassTexture, ship_diffuse, rock_diffuse, 
rock_specular, poolpedestal_diffuse, poolpedestal_specular;

std::vector<PointLightData> pointLights = {
	PointLightData(glm::vec3(-3.0f, 0.5f,  0.0f), glm::vec3(0.0f, 0.2f, 0.2f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
	PointLightData(glm::vec3(-0.0f, 0.5f,  3.0f), glm::vec3(0.2f, 0.0f, 0.2f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
	PointLightData(glm::vec3(-0.0f, 0.5f, -3.0f), glm::vec3(0.2f, 0.2f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
	PointLightData(glm::vec3(3.0f, 0.5f,  0.0f), glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f))
};

DirectionalLightData directionalLight(glm::vec3(0.0f, -.25f, -1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f));

std::vector<glm::vec3> grassPositions = {
	{-2.08378f, 0.0f,  3.15938f},
	{ 1.12966f, 0.0f, -1.22086f},
	{-1.19740f, 0.0f,  1.42118f},
	{ 0.84075f, 0.0f,  2.98022f},
	{-1.39828f, 0.0f, -1.06464f},
	{-0.56559f, 0.0f, -3.51875f},
	{ 1.77074f, 0.0f, -3.30696f},
	{-3.39814f, 0.0f,  1.32940f},
	{-3.27229f, 0.0f, -2.35374f},
	{ 2.83754f, 0.0f,  3.35755f},
	{ 2.30379f, 0.0f,  1.31363f},
	{ 3.13379f, 0.0f, -1.28161f}
};

SimpleMesh* lightBulb;

unsigned int lightProgramId;
unsigned int skyboxProgramId;
unsigned int waterProgramId;
unsigned int grassProgramId;
unsigned int flagProgramId;
unsigned int screenProgramId;
unsigned int gaussianBlurShaderProgram;
unsigned int flagPoleProgramId;
unsigned int rockProgramId;
unsigned int poolAndPedestalProgramId;
unsigned int shipProgramId;

//unsigned int basicProgramId;

SimpleBoundingBox waterRegion(glm::vec3(12, -4.5, 0), glm::vec3(6, 3.5f, 8));

bool cameraInsideWater;

#pragma endregion

// Put your variables inside this region
#pragma region Assignment Variables

#pragma endregion

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0) return;

	currentScreenWidth = width;
	currentScreenHeight = height;
	glViewport(0, 0, width, height);
	camera.setViewportSize(width, height);
}


unsigned int createRGABTexture(int width, int height)
{

	unsigned int handle;

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	return handle;



}




void initFrameBuffers()
{
	// Repeat Week 9
	// Use sceneFBO, sceneFBO_colourBuffers and sceneFBO_depthBuffer
	// but with 2 colour attachments

	//generate the framebuffer object
	glGenFramebuffers(1, &sceneFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };

	//color texture attachment
	glGenTextures(2, sceneFBO_colourBuffers);

	for (int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, sceneFBO_colourBuffers[i]);
		//sceneFBO_colourBuffers[i] = createRGBTexture(currentScreenWidth, currentScreenHeight);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, currentScreenWidth, currentScreenHeight, 0, GL_RGB, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, sceneFBO_colourBuffers[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "fb Complete!" << std::endl;

		}
		else
		{

			std::cout << "fb not Complete!" << std::endl;
		}

	}

	//Depth buffer initialize
	glDrawBuffers(2, attachments);

	glGenTextures(1, &sceneFBO_depthBuffer);
	glBindTexture(GL_TEXTURE_2D, sceneFBO_depthBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, currentScreenWidth, currentScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sceneFBO_depthBuffer, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//copy buffer
	glGenTextures(1, &buffer_copy);
	glBindTexture(GL_TEXTURE_2D, buffer_copy);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, currentScreenWidth, currentScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void initGaussionEffect1()
{
	glGenFramebuffers(2, gBlurFBOs);


	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gBlurFBOs[i]);
		gBlurFBO_colourBuffers[i] = createRGABTexture(currentScreenWidth, currentScreenHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBlurFBO_colourBuffers[i], 0);
		// do stuff on framebuffer[i], texturebuffer[i]
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "blur Complete!" << std::endl;

		}
		else
		{

			std::cout << "not Complete!" << std::endl;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (appFocused)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			lastCursorPos.x = currentScreenWidth / 2.0f;
			lastCursorPos.y = currentScreenHeight / 2.0f;
			glfwSetCursorPos(window, lastCursorPos.x, lastCursorPos.y);
			appFocused = false;
		}
		else
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	if (key == 'R' && action == GLFW_RELEASE) {
		// Do stuff
	}
}

void init()
{
	printf("Name: Chong Kang Rui\n");
	printf("Student Id: 0128391\n");
	printf("Camera Controls: WASDQE\n");
	printf("Insert anything else here\n\n");

	camera.setViewportSize(currentScreenWidth, currentScreenHeight);
	camera.setPosition(0, 4, 12);

	

	// Meshes ----------------------------------------------------------------------
	skybox = MeshUtils::makeSkybox();
	lightBulb = MeshUtils::simpleObjLoader("../assets/models/icosphere.obj");
	pool = MeshUtils::simpleObjLoader("../assets/models/pool.obj");
	pedestal = MeshUtils::simpleObjLoader("../assets/models/pedestal.obj");
	flagPole = MeshUtils::simpleObjLoader("../assets/models/pole.obj");
	flag = MeshUtils::simpleObjLoader("../assets/models/flag.obj");
	grass = MeshUtils::simpleObjLoader("../assets/models/grass.obj");
	rocks = MeshUtils::simpleObjLoader("../assets/models/rocks.obj");
	water = MeshUtils::simpleObjLoader("../assets/models/water.obj");
	ship = MeshUtils::simpleObjLoader("../assets/models/ship.obj");
	quad = MeshUtils::makeQuad();
	// -----------------------------------------------------------------------------

	// Shader Programs -------------------------------------------------------------
	//loadShaderProgram(&basicProgramId, "../assets/shaders/basic.vert", "../assets/shaders/basic.frag", "Basic");
	loadShaderProgram(&skyboxProgramId, "../assets/shaders/skybox.vert", "../assets/shaders/skybox.frag", "Skybox");
	loadShaderProgram(&lightProgramId, "../assets/shaders/light.vert", "../assets/shaders/light.frag", "Light");
	loadShaderProgram(&flagProgramId, "../assets/shaders/flag.vert", "../assets/shaders/flag.frag", "Flag");
	loadShaderProgram(&waterProgramId, "../assets/shaders/water.vert", "../assets/shaders/water.frag", "Water");
	loadShaderProgram(&grassProgramId, "../assets/shaders/grass.vert", "../assets/shaders/grass.frag", "Grass");
	loadShaderProgram(&screenProgramId, "../assets/shaders/screen.vert", "../assets/shaders/screen.frag", "Screen");
	loadShaderProgram(&gaussianBlurShaderProgram, "../assets/shaders/fs_gaussianBlur.vert", "../assets/shaders/fs_gaussianBlur.frag", "GaussianBlur");
	loadShaderProgram(&poolAndPedestalProgramId, "../assets/shaders/Pool.vert", "../assets/shaders/Pool.frag", "Pool");
	loadShaderProgram(&rockProgramId, "../assets/shaders/Rock.vert", "../assets/shaders/Rock.frag", "Rock");
	loadShaderProgram(&flagPoleProgramId, "../assets/shaders/FlagPole.vert", "../assets/shaders/FlagPole.frag", "FlagPole");
	loadShaderProgram(&shipProgramId, "../assets/shaders/ship.vert", "../assets/shaders/ship.frag", "Ship");
	// -----------------------------------------------------------------------------

	// Texture Loading -------------------------------------------------------------
	waterTexture_normal = TextureUtils::loadTexture("../assets/textures/normal.jpg");
	waterTexture = TextureUtils::loadTexture("../assets/textures/water.png");
	Flag = TextureUtils::loadTexture("../assets/textures/flag_diffuse.png");
	flagpole_diffuse = TextureUtils::loadTexture("../assets/textures/flagpole_diffuse.png");
	flagpole_specular = TextureUtils::loadTexture("../assets/textures/flagpole_specular.png");
	grassTexture = TextureUtils::loadTexture("../assets/textures/grass.png");
	poolpedestal_diffuse = TextureUtils::loadTexture("../assets/textures/poolpedestal_diffuse.png");
	poolpedestal_specular = TextureUtils::loadTexture("../assets/textures/poolpedestal_specular.png");
	rock_diffuse = TextureUtils::loadTexture("../assets/textures/rocks_diffuse.png");
	rock_specular = TextureUtils::loadTexture("../assets/textures/rocks_specular.png");
	ship_diffuse = TextureUtils::loadTexture("../assets/textures/ship_diffuse.png");

	// -----------------------------------------------------------------------------
    initFrameBuffers();
	initGaussionEffect1();


	// Render Settings
	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
}

void applyUniforms(unsigned int programId)
{
	glUniform1f(glGetUniformLocation(programId, "time"), appTime);
	glUniform2f(glGetUniformLocation(programId, "resolution"), currentScreenWidth, currentScreenHeight);

	glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_FALSE, &camera.getProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programId, "view"), 1, GL_FALSE, &camera.getViewMatrix()[0][0]);

	glUniform3fv(glGetUniformLocation(programId, "viewPos"), 1, &camera.getPosition()[0]);

	for (unsigned int i = 0; i < pointLights.size(); i++)
	{
		glUniform3fv(glGetUniformLocation(programId, std::string("pointLights[" + std::to_string(i) + "].position").c_str()), 1, &pointLights[i].getPosition()[0]);
		glUniform3fv(glGetUniformLocation(programId, std::string("pointLights[" + std::to_string(i) + "].ambient").c_str()), 1, &pointLights[i].ambient[0]);
		glUniform3fv(glGetUniformLocation(programId, std::string("pointLights[" + std::to_string(i) + "].diffuse").c_str()), 1, &pointLights[i].diffuse[0]);
		glUniform3fv(glGetUniformLocation(programId, std::string("pointLights[" + std::to_string(i) + "].specular").c_str()), 1, &pointLights[i].specular[0]);
		glUniform1f(glGetUniformLocation(programId, std::string("pointLights[" + std::to_string(i) + "].constant").c_str()), 1.0f);
		glUniform1f(glGetUniformLocation(programId, std::string("pointLights[" + std::to_string(i) + "].linear").c_str()), 0.09f);
		glUniform1f(glGetUniformLocation(programId, std::string("pointLights[" + std::to_string(i) + "].quadratic").c_str()), 0.032f);
	}
	
	glUniform3fv(glGetUniformLocation(programId, "directionalLight.direction"), 1, &directionalLight.getDirection()[0]);
	glUniform3fv(glGetUniformLocation(programId, "directionalLight.ambient"), 1, &directionalLight.ambient[0]);
	glUniform3fv(glGetUniformLocation(programId, "directionalLight.diffuse"), 1, &directionalLight.diffuse[0]);
	glUniform3fv(glGetUniformLocation(programId, "directionalLight.specular"), 1, &directionalLight.specular[0]);

	
}

void drawSkybox()
{
	glUseProgram(skyboxProgramId);

	// For skybox we want to use view matrix, but without translation
	glm::mat4 view = camera.getViewMatrix();
	view[3][0] = view[3][1] = view[3][2] = 0.0f;

	glUniformMatrix4fv(glGetUniformLocation(skyboxProgramId, "projection"), 1, GL_FALSE, &camera.getProjectionMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgramId, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgramId, "model"), 1, GL_FALSE, &glm::mat4(1)[0][0]);

	// Skybox cube is inside-face
	// Disable depth test when rendering skybox so that scene
	// objects are rendered properly.
	glDisable(GL_DEPTH_TEST);
	skybox->draw();
	glEnable(GL_DEPTH_TEST);
}

void drawFlagPole()
{
	glUseProgram(flagPoleProgramId);
	applyUniforms(flagPoleProgramId);
	glUniformMatrix4fv(glGetUniformLocation(flagPoleProgramId, "model"), 1, GL_FALSE, &glm::mat4(1.0)[0][0]);

	glUniform1f(glGetUniformLocation(flagPoleProgramId, "material.shininess"), 96.0f);
	glUniform1i(glGetUniformLocation(flagPoleProgramId, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(flagPoleProgramId, "material.specular"), 1);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, flagpole_diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, flagpole_specular);

	flagPole->draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawFlag()
{
	glUseProgram(flagProgramId);
	applyUniforms(flagProgramId);
	glUniformMatrix4fv(glGetUniformLocation(flagProgramId, "model"), 1, GL_FALSE, &glm::mat4(1.0)[0][0]);

	glUniform1f(glGetUniformLocation(flagProgramId, "material.shininess"), 32);
	glUniform1i(glGetUniformLocation(flagProgramId, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(flagProgramId, "material.specular"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Flag);
	

	flag->draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
}

void drawPoolAndPedestal()
{
	glUseProgram(poolAndPedestalProgramId);
	applyUniforms(poolAndPedestalProgramId);
	glUniformMatrix4fv(glGetUniformLocation(poolAndPedestalProgramId, "model"), 1, GL_FALSE, &glm::mat4(1.0)[0][0]);

	glUniform1f(glGetUniformLocation(poolAndPedestalProgramId, "material.shininess"), 32);
	glUniform1i(glGetUniformLocation(poolAndPedestalProgramId, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(poolAndPedestalProgramId, "material.specular"), 1);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, poolpedestal_diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, poolpedestal_specular);


	pedestal->draw();


	pool->draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawPoolRocks()
{
	glUseProgram(rockProgramId);
	applyUniforms(rockProgramId);
	glUniformMatrix4fv(glGetUniformLocation(rockProgramId, "model"), 1, GL_FALSE, &glm::mat4(1.0)[0][0]);

	glUniform1f(glGetUniformLocation(rockProgramId, "material.shininess"), 32);
	glUniform1i(glGetUniformLocation(rockProgramId, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(rockProgramId, "material.specular"), 1);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rock_diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rock_specular);
	rocks->draw();

	
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawShip()
{
	glUseProgram(shipProgramId);
	applyUniforms(shipProgramId);
	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(11.0f, -1.0f, -4.0f));
	model = glm::rotate(model, glm::radians(-45.0f), { 0.0f,1.0f,0.0f });
	model = glm::scale(model, { 0.3,0.3,0.3 });
	glUniformMatrix4fv(glGetUniformLocation(shipProgramId, "model"), 1, GL_FALSE, &model[0][0]);

	glUniform1f(glGetUniformLocation(shipProgramId, "material.shininess"), 32);
	glUniform1i(glGetUniformLocation(shipProgramId, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shipProgramId, "material.specular"), 1);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ship_diffuse);

	ship->draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawGrass()
{
	glUseProgram(grassProgramId);
	applyUniforms(grassProgramId);

	for (unsigned int i = 0; i < grassPositions.size(); i++)
	{
		glm::mat4 model = glm::translate(glm::mat4(1), grassPositions[i]);

		
		glUniformMatrix4fv(glGetUniformLocation(grassProgramId, "model"), 1, GL_FALSE, &model[0][0]);

		glUniform1f(glGetUniformLocation(grassProgramId, "material.shininess"), 32);
		glUniform1i(glGetUniformLocation(grassProgramId, "material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(grassProgramId, "material.specular"), 1);



		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTexture);


		grass->draw();


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);


	}
	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

}
	

void drawWater()
{
	
	glUseProgram(waterProgramId);
	applyUniforms(waterProgramId);
	glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(12.0f, 0.0f, 0.0f));
	
	glUniformMatrix4fv(glGetUniformLocation(waterProgramId, "model"), 1, GL_FALSE, &model[0][0]);
	/*glUniform3fv(glGetUniformLocation(waterProgramId, "material.diffuse"), 1, &glm::vec3(1, 0.5, 0)[0]);
	glUniform3fv(glGetUniformLocation(waterProgramId, "material.specular"), 1, &glm::vec3(1, 1, 1)[0]);
	glUniform1f(glGetUniformLocation(waterProgramId, "material.shininess"), 32.0f);*/

	glDisable(GL_CULL_FACE);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	glUniform1i(glGetUniformLocation(waterProgramId, "Opaquetexture"), 0);
	glUniform1i(glGetUniformLocation(waterProgramId, "DistortionTexture"), 1);

	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterTexture_normal);

	water->draw();
	
	glEnable(GL_CULL_FACE);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void renderOpaques()
{
	drawFlagPole();
	drawFlag();
	drawLightBulbs();
	drawPoolAndPedestal();
	drawPoolRocks();
	drawShip();
}

void renderAlphaTests()
{
	drawGrass();
}

void renderAlphaBlends()
{
	drawWater();
}

void draw()
{

	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawSkybox();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);

	

	renderOpaques();
	renderAlphaTests();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,buffer_copy);
	glReadBuffer(sceneFBO);
	glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,currentScreenWidth,currentScreenHeight,0);

	glDepthMask(GL_FALSE); glEnable(GL_BLEND);
	renderAlphaBlends();
	glDepthMask(GL_TRUE); glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(gaussianBlurShaderProgram); // Change to different shader program
	unsigned int count = 5;


	for (unsigned int i = 0; i < count; i++)
	{

		glBindFramebuffer(GL_FRAMEBUFFER, gBlurFBOs[0]);
		glBindTexture(GL_TEXTURE_2D, i == 0 ? sceneFBO_colourBuffers[1] : gBlurFBO_colourBuffers[1]);
		glUniform1i(glGetUniformLocation(gaussianBlurShaderProgram, "horizontal"), true);
		quad->draw();


		glBindFramebuffer(GL_FRAMEBUFFER, gBlurFBOs[1]);
		glBindTexture(GL_TEXTURE_2D, gBlurFBO_colourBuffers[0]);
		glUniform1i(glGetUniformLocation(gaussianBlurShaderProgram, "horizontal"), false);
		quad->draw();

	}



	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(screenProgramId);
	glUniform1f(glGetUniformLocation(screenProgramId, "time"), appTime);
	glUniform1i(glGetUniformLocation(screenProgramId, "colorAttachment0"), 0);
	glUniform1i(glGetUniformLocation(screenProgramId, "colorAttachment1"),1);
	glUniform1i(glGetUniformLocation(screenProgramId, "blurAttachment"), 2);
	glUniform1i(glGetUniformLocation(screenProgramId, "CameraInWater"), cameraInsideWater);

	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneFBO_colourBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sceneFBO_colourBuffers[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gBlurFBO_colourBuffers[1]);
	
	quad->draw();
	glBindTexture(GL_FRONT_AND_BACK, GL_FILL);


	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	
}

int main(void)
{
	// Using C++11 lambda expression
	glfwSetErrorCallback([](int error, const char* description) {fprintf(stderr, "Error: %s\n", description);});

	if (!glfwInit())
		exit(EXIT_FAILURE);

	// Set OpenGL context on GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "XBGT2094 Assignment 3", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	currentScreenWidth = SCREEN_WIDTH;
	currentScreenHeight = SCREEN_HEIGHT;

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}

	init();

	float prevTime = 0;
	while (!glfwWindowShouldClose(window))
	{
		appTime = glfwGetTime();
		float dt = appTime - prevTime;
		prevTime = appTime;

		cameraInsideWater = waterRegion.isInside(camera.getPosition());

		// Camera controls ----------------------------

		if (appFocused)
		{
			float mv = 4.0f;
			glm::vec3 moveOffset(0.0f);

			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) mv *= 4.0f; // Faster when LeftShift is held

			if (glfwGetKey(window, 'A')) moveOffset.x -= mv;
			if (glfwGetKey(window, 'D')) moveOffset.x += mv;
			if (glfwGetKey(window, 'W')) moveOffset.z -= mv;
			if (glfwGetKey(window, 'S')) moveOffset.z += mv;
			if (glfwGetKey(window, 'Q')) moveOffset.y += mv;
			if (glfwGetKey(window, 'E')) moveOffset.y -= mv;

			moveOffset *= dt;
			camera.translateLocal(moveOffset.x, 0.0f, moveOffset.z);
			camera.translate(0.0f, moveOffset.y, 0.0f);
		}

		// -----------------------------------------------------

		updateLights();
		draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void loadShaderProgram(unsigned int* programPtr, const std::string& vertPath, const std::string& fragPath, const std::string& shaderName)
{
	std::string errorString;
	printf("Loading '%s' shader program... ", shaderName.c_str());
	unsigned int newShaderId = ShaderUtils::InitializeShaderProgram(vertPath, fragPath, errorString);
	if (newShaderId != 0)
	{
		printf("\x1b[32mSuccess\x1b[0m\n");

		glDeleteProgram(*programPtr);
		*programPtr = newShaderId;
	}
	else
	{
		printf("\x1b[31mFailed\n\x1b[33m%s\x1b[0m", errorString.c_str());
	}
}

#pragma region GLFW Callbacks

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	mousePosition.x = xpos;
	// OpenGL viewport origin is bottom left, while GLFW window origin is upper left
	// Hence the subtraction.
	mousePosition.y = currentScreenHeight - ypos;

	if (!appFocused) {
		lastCursorPos.x = xpos; lastCursorPos.y = ypos;
		return;
	}

	if (lastCursorPos.x == -1 && lastCursorPos.y == -1)
	{
		lastCursorPos.x = xpos; lastCursorPos.y = ypos;
		return;
	}

	glm::vec2 offset = lastCursorPos - glm::vec2(xpos, ypos);
	lastCursorPos.x = xpos; lastCursorPos.y = ypos;

	camera.rotate(offset.x * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
	camera.rotateLocal(offset.y * 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (!appFocused)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		appFocused = true;
	}
}

#pragma endregion

#pragma region DO NOT CHANGE

void updateLights()
{
	for (unsigned int i = 0; i < pointLights.size(); i++)
	{
		float dir = i % 2 == 0 ? 1 : -1;
		pointLights[i].setRotation(dir * (appTime * 0.6f + i));
		pointLights[i].position.y = 0.5f + (sinf(appTime * 0.2f + i) * 0.5 + 0.5) * 9;
	}

	directionalLight.setRotation(appTime);
}

void drawLightBulbs()
{
	glUseProgram(lightProgramId);
	applyUniforms(lightProgramId);

	for (unsigned int i = 0; i < pointLights.size(); i++)
	{
		float dir = i % 2 == 0 ? 1 : -1;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, pointLights[i].getPosition());
		model = glm::scale(model, glm::vec3(0.15f));

		glUniformMatrix4fv(glGetUniformLocation(lightProgramId, "model"), 1, GL_FALSE, &model[0][0]);
		glUniform3fv(glGetUniformLocation(lightProgramId, "colour"), 1, &pointLights[i].diffuse[0]);

		lightBulb->draw();
	}
}

#pragma endregion