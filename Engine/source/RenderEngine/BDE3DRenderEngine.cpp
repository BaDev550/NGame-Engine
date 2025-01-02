#include <RenderEngine/BDE3DRenderEngine.h>
#include <iostream>

using std::cout;
using std::endl;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX;
float lastY;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

unsigned int quadVAO;
int previousKeyState;

void BDE3DE::start(unsigned int wHeight, unsigned int wWidth, const char* wTitle)
{
	w_Height = wHeight;
	w_Width = wWidth;
	lastX = static_cast<float>(w_Width / 2);
	lastX = static_cast<float>(w_Height / 2);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	if (!glfwInit()) {
		cout << "FAILED TO LOAD GLFW" << endl;
		return;
		glfwTerminate();
	}
	cout << "GLFW Init success" << endl;

	window = glfwCreateWindow(w_Width, w_Height, wTitle, NULL, NULL);
	if (window == NULL) {
		cout << "FAILED TO LOAD WINDOW" << endl;
		return;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	previousKeyState = GLFW_RELEASE;

	cout << "Window Created" << endl;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "FAILED TO LOAD GLAD" << endl;
		return;
		glfwTerminate();
	}
	glViewport(0, 0, w_Width, w_Height);
	glEnable(GL_DEPTH_TEST);

	cout << "Glad Init success" << endl;

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "[ERROR]OpenGL error: " << err << std::endl;
	}

	skyBox = new SkyBox();
	CreateFBO();
	RManager->GetShader("ScreenShader").Use();
	RManager->GetShader("ScreenShader").SetInteger("texture1", 0);
}

void BDE3DE::render(std::map<std::string, GameObject*> obj)
{
	float aspectRatio = (float)w_Height / (float)w_Width;

	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	RManager->GetShader("SimpleShader").Use();
	RManager->GetShader("SimpleShader").SetMatrix4("model", matrixes.model);
	RManager->GetShader("SimpleShader").SetMatrix4("view", matrixes.view);
	RManager->GetShader("SimpleShader").SetMatrix4("projection", matrixes.projection);

	renderModels(obj);
	renderSkyBox();
	renderRaycast();

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

}

void BDE3DE::renderModels(std::map<std::string, GameObject*> obj)
{
	for (auto& object : obj) {
		if (!object.second->IsVisible) { continue; }

		matrixes.model = glm::mat4(1.0f);
		matrixes.model = glm::translate(matrixes.model, object.second->Position);
		matrixes.model = glm::rotate(matrixes.model, glm::radians(object.second->Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // Rotate around X axis
		matrixes.model = glm::rotate(matrixes.model, glm::radians(object.second->Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y axis
		matrixes.model = glm::rotate(matrixes.model, glm::radians(object.second->Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));   // Rotate around Z axis
		matrixes.model = glm::scale(matrixes.model, object.second->Size);

		matrixes.view = camera.GetViewMatrix();
		matrixes.projection = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);

		RManager->GetShader("DefaultShader").Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox->cubemapTexture);
		RManager->GetShader("DefaultShader").SetInteger("skybox", 0);

		RManager->GetShader("DefaultShader").SetMatrix4("model", matrixes.model);
		RManager->GetShader("DefaultShader").SetMatrix4("view", matrixes.view);
		RManager->GetShader("DefaultShader").SetMatrix4("projection", matrixes.projection);
		RManager->GetShader("DefaultShader").SetVector3f("viewPos", camera.Position);
		RManager->GetShader("DefaultShader").SetVector3f("light.position", glm::vec3(0.0f, 10.0f, 10.0f));
		RManager->GetShader("DefaultShader").SetVector3f("light.color", glm::vec3(1.0f, 1.0f, 1.0f));

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL Error after rendering model: " << error << std::endl;
		}

		object.second->Draw();
	}
}

void BDE3DE::renderSkyBox()
{
	matrixes.view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	matrixes.projection = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.01f, 100.0f);

	RManager->GetShader("SkyBoxShader").Use();
	RManager->GetShader("SkyBoxShader").SetMatrix4("view", matrixes.view);
	RManager->GetShader("SkyBoxShader").SetMatrix4("projection", matrixes.projection);

	glDepthMask(GL_FALSE);
	skyBox->Draw();
	glDepthMask(GL_TRUE);
}

void BDE3DE::renderRaycast()
{
	matrixes.view = camera.GetViewMatrix();
	matrixes.projection = glm::perspective(glm::radians(camera.Zoom), aspectRatio, 0.1f, 100.0f);

	RManager->GetShader("DebugShader").Use();
	RManager->GetShader("DebugShader").SetMatrix4("projection", matrixes.projection);
	RManager->GetShader("DebugShader").SetMatrix4("view", matrixes.view);
}

void BDE3DE::quit()
{
	delete RManager;
	delete skyBox;
	glfwDestroyWindow(window);
	glfwTerminate();
}

Camera BDE3DE::GetCamera()
{
	return camera;
}

void BDE3DE::ProcessKeyboard()
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

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && previousKeyState == GLFW_RELEASE) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			firstMouse = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
			std::cout << "Mouse Mode changed to normal" << std::endl;
		}
		else {
			firstMouse = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			std::cout << "Mouse Mode changed to capture" << std::endl;
		}
	}
	previousKeyState = glfwGetKey(window, GLFW_KEY_TAB);
}

void BDE3DE::CreateFBO()
{
	unsigned int quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w_Width, w_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w_Width, w_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) { return; }

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void RenderGizmo(Mesh& gizmo, glm::mat4 modelMatrix)
{
	ResourceManager::GetShader("DefaultShader").SetVector3f("color", glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 model = glm::mat4(1.0f);
	model = model * modelMatrix;
	ResourceManager::GetShader("DefaultShader").SetMatrix4("model", model);

	gizmo.Draw();
}