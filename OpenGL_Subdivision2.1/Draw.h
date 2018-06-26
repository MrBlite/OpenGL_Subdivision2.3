#pragma once
#include "assistance.h"
#include "Model.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// timing
double deltaTime = 0.0f;	// time between current frame and last frame
double lastFrame = 0.0f;
// lighting
glm::vec3 lightPos(1.2f, 1.0f, -20.0f);
#include "functions.h"

int drawFace(SUB_RESULT_USAGE USAGE, vector<GLdouble> v, vector<unsigned int>& degree, vector<unsigned short> & out_indices);
int drawOther(SUB_RESULT_USAGE USAGE, vector<GLdouble> v, vector<unsigned int>& degree);
int draw(SUB_RESULT_USAGE USAGE, vector<GLdouble> v, vector<unsigned int>& degree, vector<unsigned short> & out_indices)
{

	switch (USAGE)
	{
	case DRAW_VERTEX: case DRAW_FRAME: case DRAW_POLYFACE: case DRAW_POLYLINE:
		return drawOther(USAGE, v, degree);
		break;
	case DRAW_TRIFACE:
		return drawFace(USAGE, v, degree, out_indices);
		break;
	default:
		break;
	}

}

//顶点数据包含了法向量
int drawFace(SUB_RESULT_USAGE USAGE, vector<GLdouble> v, vector<unsigned int>& degree, vector<unsigned short> & out_indices)
{
#pragma region prepare
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 500, 100);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//注册回调函数
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 初始化glew(管理OpenGL的函数指针)
	// -------------------------------
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	//只渲染一面
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(3.0f);
	glEnable(GL_DEPTH_TEST);//深度测试
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
#pragma endregion


	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	Shader shaderCube("shader.vert", "shader.frag");
	model = glm::translate(model, glm::vec3(0.0, 0.0, -2.0));

	//object
	GLuint VAO[1];
	GLuint VBO[1];
	GLuint EBO[1];

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);

	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(GLdouble), &v[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, out_indices.size() * sizeof(unsigned short), &out_indices[0], GL_STATIC_DRAW);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// position attribute
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 6 * sizeof(double), (void*)(3 * sizeof(double)));
	glEnableVertexAttribArray(1);

	int num_vertex_used = v.size() / 3 / 2;

	//渲染循环Render Loop
	//-------------------
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//DEPTH_BUFFER_BIT位来清除深度缓冲：
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glBindVertexArray(VAO[0]);
		shaderCube.use();

		lightPos = camera.Position-(0.0f,0.0f,-10.0f);
		shaderCube.setVec3("light.position", lightPos);
		shaderCube.setVec3("viewPos", camera.Position);

		// light properties
		glm::vec3 lightColor(1.0,1.0,1.0);
		//lightColor.x = sin(glfwGetTime() * 2.0f);
		//lightColor.y = sin(glfwGetTime() * 0.7f);
		//lightColor.z = sin(glfwGetTime() * 1.3f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		shaderCube.setVec3("light.ambient", ambientColor);
		shaderCube.setVec3("light.diffuse", diffuseColor);
		shaderCube.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		shaderCube.setVec3("material.ambient", 0.2125f, 0.1275f, 0.054f);
		shaderCube.setVec3("material.diffuse", 0.714f, 0.4284f, 0.18144f);
		shaderCube.setVec3("material.specular", 0.393548f, 0.271906f, 0.166721f); // specular lighting doesn't have full effect on this object's material
		shaderCube.setFloat("material.shininess", 0.2f);

		view = camera.GetViewMatrix();
		projection = glm::perspective((GLfloat)glm::radians(camera.Zoom), (GLfloat)(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
		shaderCube.setMat4("model", model);
		shaderCube.setMat4("view", view);
		shaderCube.setMat4("projection", projection);

		int sum = 0;

		switch (USAGE)
		{
		case DRAW_TRIFACE:
			//glDrawArrays(GL_TRIANGLES, 0, num_vertex_used);
			glDrawElements(GL_TRIANGLES, out_indices.size(), GL_UNSIGNED_SHORT, 0);
			break;
		case DRAW_POLYFACE:
			for (int i = 0; i < degree.size(); i++)
			{
				glDrawArrays(GL_TRIANGLE_FAN, sum, degree[i]);
				sum += degree[i];
			}
			break;
		default:
			cerr << "sorry, but we can't generate this kind of array" << endl;
			break;
		}
		glfwSwapBuffers(window);//检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
		glfwPollEvents();//交换颜色缓冲
	}

	glDeleteVertexArrays(1, VAO);
	//glDeleteBuffers(1, EBO);
	glDeleteBuffers(1, VBO);
	glfwTerminate();//释放资源

}

//顶点数据未包含法向量
int drawOther(SUB_RESULT_USAGE USAGE, vector<GLdouble> v, vector<unsigned int>& degree)
{
	return 0;
}