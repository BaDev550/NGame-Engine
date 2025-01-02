#ifndef BDENGINE_H
#define BDENGINE_H
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <map>
#include <filesystem>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "GameObject.h"
#include "Components/StaticMeshComponent/CStaticMeshComponent.h"
#include "Components/BoxCollisionComponent/CBoxColliderComponent.h"
#include "Components/RigidBodyComponent/CRigidBodyComponent.h"
#include <GameEngine/Scene/Scene.h> 
#include <GameEngine/Scene/Raycast/RayCast.h>
#include <GameEngine/Scene/Debug/DebugDraw.h>

#include <RenderEngine/BDE3DRenderEngine.h>

#include <GameEngine/Components/ScriptingComponent/CScriptingComponent.h>

class Scene;
class BDE3DE;

enum class GameState {
	EditorMode,
	GameMode
};

enum class EditorState {
	Playing,
	Paused
};

class BDEngine
{
private:
	std::unordered_map<std::string, std::vector<std::string>> componentCategories = {
		{"Physics", {"Rigid Body", "Box Collision"}},
		{"Rendering", {"Static Mesh"}},
		{"Scripting", {"BDScript"}}
	};

	ResourceManager* RManager;

	std::string sceneName{ "scene.json" };
	std::string GameName{ "BDEngine" };
	unsigned int GameWindowWidth{ 800 }, GameWindowHeight{ 800 };

	GameState currentState{ GameState::EditorMode };
	EditorState editorState{ EditorState::Paused };
	//GameObject* activeCamera;
public:
	BDEngine() {};

	Scene* scene = NULL;
	BDE3DE* BDRenderEngine = NULL;
	RayCast* Raycast = NULL;

	bool bDebugMode = false;

	void createGame(std::string gameName, unsigned int wWidth, unsigned int wHeight);
	void checkRayInteraction(const glm::vec2& mousePos, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
	void update();
	void render();

	void ProcessInput();
private:
	void RenderGUI();
	void RenderMenuBar();
	void RenderSceneBrowser();
	void RenderViewport();
	void ShowComponentAdderUI();
protected:
	void start();
	void quit();
};
#endif // !BDENGINE_H