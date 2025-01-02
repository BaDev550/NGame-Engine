#ifndef BDE3DE_H
#define BDE3DE_H
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include <ResourceManager.h>
#include <GameEngine/GameObject.h>
#include "Camera/Camera.h"
#include "SkyBox/SkyBox.h"

#include <GameEngine/BDEngine.h>

struct Matrixes
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

class BDE3DE
{
public:
	BDE3DE() {};

	void start(unsigned int wHeight, unsigned int wWidth, const char* wTitle);
	void render(std::map<std::string, GameObject*> obj);
	void renderModels(std::map<std::string, GameObject*> obj);
	void renderSkyBox();
	void renderRaycast();
	void quit();

	unsigned int textureColorbuffer;
	unsigned int framebuffer;

	Matrixes matrixes;
protected:
	GLFWwindow* window;
	SkyBox* skyBox;
	ResourceManager* RManager;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	unsigned int w_Height = 800, w_Width = 800;
	float aspectRatio = (float)w_Width / (float)w_Height;

private:
	void CreateFBO();

public:
	void ProcessKeyboard();

	Camera GetCamera();
	__forceinline GLFWwindow* GetWindow() { return window; }
	__forceinline float GetDeltaTime() { return deltaTime; }
	__forceinline Matrixes GetMatrixes() { return matrixes; }
};

#endif // !BDE3D_H

