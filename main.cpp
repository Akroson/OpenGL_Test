#include <iostream>

#include <glad/glad.h>
#include <glad/glad.c>
#include <GLFW/glfw3.h>

#include "render_math.h"
#include "shader_s.h"

struct camera_state
{
	v3 Pos;
	v3 Dir;
	v3 Up;
	v3 Right;
	v3 WorldUp;

	float Yaw;
	float Pitch;
};

float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top   
};

float cubeVertices[] = {
	// positions       
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f
};


static camera_state Camera;

int SCR_WIDTH = 1024;
int SCR_HEIGHT = 1024;

bool FirstMouse = true;

bool GlobalRanning = false;

float DeltaTime = 0.0f;
float LastFrame = 0.0f;

inline mat4
GetCameraView(camera_state *Camera)
{
	return LookAt(Camera->Pos, Camera->Pos + Camera->Dir, Camera->Up);
}

#define MOUSE_SENSATIVITY 0.0007f
void
UpdateCameraViewDir(camera_state *Camera)
{
	v3 Front;

	float CosP = cosf(Camera->Pitch);
	float SinY = sinf(Camera->Yaw);
	float CosY = cosf(Camera->Yaw);

	Front.y = sinf(Camera->Pitch);
	Front.x = CosY * CosP;
	Front.z = SinY * CosP;
	Camera->Dir = normalize(Front);

	Camera->Right = cross(Camera->Dir, Camera->WorldUp);
	Camera->Up = cross(Camera->Right, Camera->Dir);
}
void
ProcessMouseMovement(camera_state *Camera, float XOffset, float YOffset)
{
	XOffset *= MOUSE_SENSATIVITY;
	YOffset *= MOUSE_SENSATIVITY;

	Camera->Yaw += XOffset;
	Camera->Pitch += YOffset;

	UpdateCameraViewDir(Camera);
}

void
ProcessCameraMovement(camera_state *Camera, int VkKey)
{
	float Velocity = DeltaTime * 2.5f;
	switch (VkKey)
	{
	case GLFW_KEY_W:
	{
		Camera->Pos += Camera->Dir * Velocity;
	}break;

	case GLFW_KEY_S:
	{
		Camera->Pos -= Camera->Dir * Velocity;
	}break;

	case GLFW_KEY_A:
	{
		Camera->Pos -= Camera->Right * Velocity;
	}break;

	case GLFW_KEY_D:
	{
		Camera->Pos += Camera->Right * Velocity;
	}break;
	}
}

void ProcessInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		ProcessCameraMovement(&Camera, GLFW_KEY_W);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		ProcessCameraMovement(&Camera, GLFW_KEY_S);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		ProcessCameraMovement(&Camera, GLFW_KEY_A);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		ProcessCameraMovement(&Camera, GLFW_KEY_D);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

float LastX = 0;
float LastY = 0;

void mouse_callback(GLFWwindow* Window, double InputXpos, double InputYpos)
{
	float Xpos = (float)InputXpos;
	float Ypos = (float)InputYpos;

	if (FirstMouse)
	{
		LastX = Xpos;
		LastY = Ypos;
		FirstMouse = false;
	}

	float Xoffset = Xpos - LastX;
	float Yoffset = LastY - Ypos;

	LastX = Xpos;
	LastY = Ypos;

	ProcessMouseMovement(&Camera, Xoffset, Yoffset);
}

void scroll_callback(GLFWwindow* Window, double Xoffset, double Yoffset)
{
}

int
main(int a, char **b)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	Camera.Dir = V3(0, 0, -1.0f);
	Camera.Pos = V3(0, 0, 4.0f);
	Camera.Yaw = AngelToRadian(-90.0f);
	Camera.WorldUp = V3(0, 1, 0);
	UpdateCameraViewDir(&Camera);

	glEnable(GL_DEPTH_TEST);

	shader Shader("v.vert", "f.frag");

	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		DeltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		ProcessInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 View = GetCameraView(&Camera);
		mat4 Persp = Perspective(AngelToRadian(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		mat4 Model = Identity();

		Shader.Use();

		mat4 MVP = View * Model;
		MVP = Persp * MVP;

		Shader.SetMat4("MVP", MVP);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	return 0;
}