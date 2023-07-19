#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

#include "shader.h"
#include "image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include <map> 
#include "object.h"
#include "font.h"
#include "fps.h"
#include "PRT/PRT.h"
#include "SH/SH_SAMPLE.h"
#include "SH/SH_Objects/SH_BOX.h"
#include "SH/SH_Objects/SH_SPHERE.h"
#include "SH_Rotate/SH_ROTATION.h"
#include "SH/SH.h"
#include <vector>
using std::map;
using std::vector;
unsigned int GetHDRTexture(string path);
void DrawLight(Shader lightShader, unsigned int lightVAO, glm::vec3 lightColor, glm::mat4 view, glm::mat4 projection, glm::vec3 position);
unsigned int GetTexture(string path);
void normalSquare(unsigned int& VAO, unsigned int& VBO);
unsigned int loadCubemap(const vector<string>& faces, string path);
void getFps(GLFWwindow* window, float currentTime, float deltaTime);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//camera
Camera camera(glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

float lastTime = 0.0f, deltaTime = 0.0f;
double lastX, lastY;
bool firstMouse = true;

const int sqrtNumSamples = 21;
const int numSamples = sqrtNumSamples * sqrtNumSamples;
const int numBands = 4;
const int numFunctions = numBands * numBands;
float prtlight[numFunctions];
float rotatedprtlight[numFunctions];
glm::vec3 lightColor = glm::vec3(1.0f, 2.0f, 1.0f);
float theta = 30.0f;
float phi = 30.0f;
//FPS
FPS_COUNTER ourFPS;

int main() {

	//GLFW初始化
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4);//多重采样缓冲

	//GLFW窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//GLAD初始化
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//callback
	//glViewport(0, 0, 400, 300);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//VAO VBO EBO
	float vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -   ----法线----
			 -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,// 左下
			 0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,// 右下
			 0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,// 右上
			-0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, // 左上

			//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -   ----法线----
				-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,// 左下
				0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,// 右下
				 0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,// 右上
				-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,// 左上

				//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -   ----法线----
					-0.5f, 0.5f,-0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,// 左下
					-0.5f, 0.5f, 0.5f,    1.0f, 1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 右下
					 0.5f, 0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,// 右上
					0.5f, 0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 左上

					//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -   ----法线----
						-0.5f, -0.5f,-0.5f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,// 左下
						-0.5f, -0.5f, 0.5f,    1.0f, 1.0f, 0.0f,   1.0f,0.0f,   0.0f, -1.0f, 0.0f, // 右下
						 0.5f, -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,// 右上
						 0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 左上

						 //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -   ----法线----
							 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,// 左下
							 0.5f, 0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,    1.0f, 0.0f, 0.0f,// 右下
							  0.5f, 0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,// 右上
							  0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f,// 左上

							  //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -   ----法线----
								  -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,// 左下
								  -0.5f, 0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,    -1.0f, 0.0f, 0.0f,// 右下
								   -0.5f, 0.5f,  0.5f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,// 右上
								   -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f // 左上
	};
	unsigned int indices[] = {
		0, 1, 2, // 第一个三角形
		2, 3, 0,  // 第二个三角形

		6, 5, 4,
		4, 7, 6,

		8, 9, 10,
		10, 11, 8,

		14 ,13, 12,
		12, 15, 14,

		16, 17, 18,
		18, 19, 16,

		22, 21, 20,
		20, 23, 22
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	//program
	Shader frameShader("res/shader/hdrVertex.shader", "res/shader/hdrFragment.shader");
	//LIGHT
	Shader lightShader("res/shader/lightVertex.shader", "res/shader/lightFragment.shader");

	//帧缓冲
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float frameVertices[] =
	{
		-1.0, -1.0,  0.0, 0.0,
		 1.0, -1.0,  1.0, 0.0,
		 1.0,  1.0,  1.0, 1.0,
		 1.0,  1.0,  1.0, 1.0,
		-1.0,  1.0,  0.0, 1.0,
		-1.0, -1.0,  0.0, 0.0
	};

	unsigned int frameVBO;
	glGenBuffers(1, &frameVBO);
	unsigned int frameVAO;
	glGenVertexArrays(1, &frameVAO);

	glBindVertexArray(frameVAO);
	glBindBuffer(GL_ARRAY_BUFFER, frameVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), frameVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Font
	Font ourFont;

	//PRT
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	std::vector<SH_Object>PrtObjects;
	SH_BOX prtbox(glm::vec3(3.1, 0.15, 3.1), glm::vec3(4.4, 1.4, 4.4), std::make_shared<solid_color>(1.0, 0.1, 0.1), 0.9, 0.1, 8, 8, 8);
	SH_BOX prtwall1(glm::vec3(5.0, 0.1, -0.1), glm::vec3(5.1, 3.0, 5.0), std::make_shared<solid_color>(0.0, 0.0, 0.1), 0.1, 0.8, 1, 10, 10);
	SH_BOX prtwall2(glm::vec3(0.0, 0.1, -0.1), glm::vec3(5.0, 3.0, 0.0), std::make_shared<solid_color>(0.1, 0.0, 0.0), 0.1, 0.8, 10, 10, 1);
	SH_BOX prtfloor(glm::vec3(0.0, 0.0, 0.0), glm::vec3(5.0, 0.1, 5.0), std::make_shared<solid_color>(0.0, 0.1, 0.0), 0.1, 0.2, 10, 1, 10);
	SH_SPHERE prtsphere(glm::vec3(2.2, 1.1, 2.2), 1.0f, std::make_shared<solid_color>(1.0, 0.1, 1.0), 0.6, 0.8, 30, 15);
	
	PrtObjects.push_back(prtbox);
	PrtObjects.push_back(prtwall1);
	PrtObjects.push_back(prtwall2);
	PrtObjects.push_back(prtfloor);
	PrtObjects.push_back(prtsphere);

	SH_SAMPLE* samples = new SH_SAMPLE[sqrtNumSamples * sqrtNumSamples];
	if (!GenerateSHSampes(sqrtNumSamples, numBands, samples))
		std::cout << "generate samples failed!" << std::endl;

	GetObjectGlossy(PrtObjects, numSamples, numBands, samples);
	GetPTRCoffes(PrtObjects, sqrtNumSamples * sqrtNumSamples, numBands, samples, window, &ourFont, "directcoeffs_glossy_20_4.dat");
	Shader prtShader("res/shader/prtVertex.shader", "res/shader/prtFragment.shader");
	GetLightCoeffs(numSamples, numBands, samples, prtlight);

	//GLFW渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//输入
		processInput(window);

		//MSAA
		//glEnable(GL_MULTISAMPLE);

		//framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glEnable(GL_DEPTH_TEST);

		glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CCW);

		glDisable(GL_STENCIL_TEST);

		glDisable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		//view projection
		glm::mat4 view = camera.ViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		
		//prt
		RotateSHCoefficients(numBands, prtlight, rotatedprtlight, theta, phi);
		GetOutcCoeffs(PrtObjects, rotatedprtlight, lightColor, camera.Position, numSamples, numBands);
		DrawPrtObjects(prtShader, PrtObjects, view, projection, FBO, SCR_WIDTH, SCR_HEIGHT);

		//light
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		const float PI = 3.14150265359;
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(2.2, 1.1, 2.2)+8.0f * glm::vec3(cos(phi/180.0*PI) * sin(theta / 180.0 * PI), cos(theta / 180.0 * PI), sin(theta / 180.0 * PI) * sin(phi / 180.0 * PI)));
		model = glm::scale(model, glm::vec3(0.5f));
		
		lightShader.use();
		lightShader.setMat4("model", glm::value_ptr(model));
		lightShader.setMat4("view", glm::value_ptr(view));
		lightShader.setMat4("projection", glm::value_ptr(projection));
		
		lightShader.setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		
		//time
		float currentTime = (float)glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		ourFPS.Update();

		//Font: 放在最后，以便混合
		std::stringstream ss;
		ss << "FPS:" << ourFPS.GetFps();
		ourFont.RenderText(ss.str().substr(0, 10), 10.0f, 550.0f, 0.8f, glm::vec3(0.0, 0.5, 0.0));
		std::stringstream ss1;
		ss1 << "Theta:" << theta;
		ourFont.RenderText(ss1.str(), 10.0f, 500.0f, 0.8f, glm::vec3(0.0, 0.5, 0.0));
		std::stringstream ss2;
		ss2 << "Phi:" << phi;
		ourFont.RenderText(ss2.str(), 10.0f, 450.0f, 0.8f, glm::vec3(0.0, 0.5, 0.0));

		//后处理
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_BLEND);
		glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		frameShader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		frameShader.setInt("texColorBuffer", 0);

		frameShader.setFloat("exposure", 1.0f);
		glBindVertexArray(frameVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//事件检查、缓冲交换
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	delete[]samples;

	//释放资源
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.PositionMove(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.PositionMove(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.PositionMove(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.PositionMove(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.PositionMove(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		camera.PositionMove(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		theta += 50.0 * deltaTime;
		theta = glm::min(theta, 180.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		theta -= 50.0 * deltaTime;
		theta = glm::max(theta, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		phi += (50.0 * deltaTime);
		if (phi > 360.0f)
			phi -= 360.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		phi -= (50.0 * deltaTime);
		if (phi < 0.0f)
			phi += 360.0f;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ViewMove(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.FovMove(yoffset);
}