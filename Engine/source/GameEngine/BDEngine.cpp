#include "GameEngine/BDEngine.h"

glm::vec3 rayDir;
GameObject* hitObject;
bool bHit;
bool bCanShowProps;
bool bisHoveringImGui;
bool bShowNameInput = false;
bool bBlockedInput = false;

int KeyState;

void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.ItemSpacing = ImVec2(10, 5);
    style.ScrollbarSize = 10.0f;
    
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.18f, 0.36f, 0.82f, 1.0f); 
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.42f, 0.92f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.32f, 0.72f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f); 
}

void BDEngine::createGame(std::string gameName, unsigned int wWidth, unsigned int wHeight)
{
	GameName = gameName;
	GameWindowWidth = wWidth;
	GameWindowHeight = wHeight;
	start();
}

void BDEngine::start()
{
    BDRenderEngine = new BDE3DE;
    scene = new Scene;
    Raycast = new RayCast;
	BDRenderEngine->start(GameWindowWidth, GameWindowHeight, GameName.c_str());

    KeyState = GLFW_RELEASE;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(BDRenderEngine->GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    SetupImGuiStyle();

    GameObject::RegisterComponent("Rigid Body", []() { return std::make_shared<CRigidBodyComponent>(); });
    GameObject::RegisterComponent("Box Collision", []() { return std::make_shared<CBoxCollisionComponent>(); });
    GameObject::RegisterComponent("Static Mesh", []() { return std::make_shared<CStaticMeshComponent>(); });
    GameObject::RegisterComponent("BDScript", []() { return std::make_shared<CScriptingComponent>(); });
}

void BDEngine::update()
{
    RManager->LoadShader("resources/shaders/ComplexShaders/Default.vert", "resources/shaders/ComplexShaders/Default.frag", "DefaultShader");
    RManager->LoadShader("resources/shaders/ComplexShaders/SkyBox.vert", "resources/shaders/ComplexShaders/SkyBox.frag", "SkyBoxShader");
    RManager->LoadShader("resources/shaders/Screen.vert", "resources/shaders/Screen.frag", "ScreenShader");
    RManager->LoadShader("resources/shaders/Simple.vert", "resources/shaders/Simple.frag", "SimpleShader");
    RManager->LoadShader("resources/shaders/DebugDrawing/Debug.vert", "resources/shaders/DebugDrawing/Debug.frag", "DebugShader");

    scene->CheckCollision();
    while (!glfwWindowShouldClose(BDRenderEngine->GetWindow()))
    {
        float DeltaTime = BDRenderEngine->GetDeltaTime();
        if (currentState == GameState::GameMode && editorState != EditorState::Paused) {
            scene->update(DeltaTime);
        }
        render();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    BDRenderEngine->quit();
    return;
}

void BDEngine::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    bisHoveringImGui = ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

    glfwPollEvents();

    RenderGUI();

    scene->updateTransforms();
    scene->render(*BDRenderEngine);

    if (bDebugMode) {
        glm::vec3 rayStart = BDRenderEngine->GetCamera().Position;
        glm::vec3 rayEnd = rayStart + rayDir * 100.0f;
        DebugDraw::DrawLine(rayStart, rayEnd, glm::vec3(1, 0, 0));

        for (auto& obj : scene->GameObjects) {
            GameObject* gameObject = obj.second;

            if (gameObject->GetComponent<CStaticMeshComponent>() != nullptr) {
                glm::vec3 boxMin = gameObject->GetComponent<CStaticMeshComponent>()->getMinBoundingBox(gameObject->Position);
                glm::vec3 boxMax = gameObject->GetComponent<CStaticMeshComponent>()->getMaxBoundingBox(gameObject->Position);

                if (glm::distance(BDRenderEngine->GetCamera().Position, boxMin) < 10.0f) {
                    DebugDraw::DrawAABB(boxMin, boxMax, glm::vec3(1, 1, 0));
                }
            }
        }
    }


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ProcessInput();
    glfwSwapBuffers(BDRenderEngine->GetWindow());
    glfwPollEvents();
}

void BDEngine::ProcessInput()
{
    if (!bBlockedInput) {
        BDRenderEngine->ProcessKeyboard();
        if (glfwGetMouseButton(BDRenderEngine->GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && KeyState == GLFW_RELEASE) {
            if (glfwGetInputMode(BDRenderEngine->GetWindow(), GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
                double mouseX, mouseY;
                glfwGetCursorPos(BDRenderEngine->GetWindow(), &mouseX, &mouseY);
                glm::vec2 mousePos(mouseX, mouseY);

                checkRayInteraction(mousePos, BDRenderEngine->GetMatrixes().view, BDRenderEngine->GetMatrixes().projection, BDRenderEngine->GetCamera().Position);
            }
        }
        KeyState = glfwGetKey(BDRenderEngine->GetWindow(), GLFW_MOUSE_BUTTON_LEFT);

    }
}

void BDEngine::quit()
{
    delete BDRenderEngine;
    delete scene;
    delete Raycast;
}

bool intersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& boxMin, const glm::vec3& boxMax, float& t) {
    float tmin = (boxMin.x - rayOrigin.x) / rayDir.x;
    float tmax = (boxMax.x - rayOrigin.x) / rayDir.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (boxMin.y - rayOrigin.y) / rayDir.y;
    float tymax = (boxMax.y - rayOrigin.y) / rayDir.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if (tmin > tymax || tymin > tmax) return false;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (boxMin.z - rayOrigin.z) / rayDir.z;
    float tzmax = (boxMax.z - rayOrigin.z) / rayDir.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if (tmin > tzmax || tzmin > tmax) return false;

    t = tmin > tzmin ? tmin : tzmin;
    return true;
}

void BDEngine::checkRayInteraction(
    const glm::vec2& mousePos,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos)
{
    rayDir = Raycast->getRayFromMouse(
        mousePos,
        view,
        projection,
        cameraPos
    );

    float closestHitDist = INFINITY;

    for (auto& obj : scene->GameObjects) {
        GameObject* gameObject = obj.second;

        if (gameObject->GetComponent<CStaticMeshComponent>() != nullptr) {
            glm::vec3 boxMin = gameObject->GetComponent<CStaticMeshComponent>()->getMinBoundingBox(gameObject->Position);
            glm::vec3 boxMax = gameObject->GetComponent<CStaticMeshComponent>()->getMaxBoundingBox(gameObject->Position);

            float t = 0.0f;
            bool bHit = intersectRayAABB(cameraPos, rayDir, boxMin, boxMax, t);


            if (bHit) {
                if (t < closestHitDist) {
                    if(bDebugMode)
                        DebugDraw::DrawAABB(boxMin, boxMax, glm::vec3(0, 1, 0));
                    closestHitDist = t;
                    hitObject = obj.second;
                    bCanShowProps = true;
                }
            }
        }
    }

    if (!hitObject) {
        bCanShowProps = false;
    }
}

void BDEngine::RenderGUI()
{
    RenderMenuBar();
    RenderSceneBrowser();
    RenderViewport();
}

void BDEngine::RenderViewport() {
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    float menuBarHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(windowSize.x * 0.7f, windowSize.y - menuBarHeight));
    ImGui::Begin("Viewport", nullptr, 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(intptr_t)BDRenderEngine->textureColorbuffer, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        bBlockedInput = false;
    }
    ImGui::End();
}

void BDEngine::RenderMenuBar() {
    static char newSceneName[128] = ""; 
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                scene->Save(sceneName);
            }
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) {
                if (ImGui::Begin("Save file as")) {
                    ImGui::InputText("New Scene Name", newSceneName, IM_ARRAYSIZE(newSceneName));
                    if (ImGui::Button("Save")) {
                        sceneName = newSceneName;
                        sceneName = sceneName + ".scene";
                        scene->Save(newSceneName);
                    }
                }
                ImGui::End();
            }
            if (ImGui::MenuItem("Load", "Ctrl+O")) {
                scene->Load(sceneName);
            }
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(BDRenderEngine->GetWindow(), true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo", "Ctrl+Z");
            ImGui::MenuItem("Redo", "Ctrl+Y");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Viewport")) {
            if (ImGui::MenuItem("Wireframe", "Ctrl+W")) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            if (ImGui::MenuItem("Normal View", "Ctrl+N")) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void BDEngine::RenderSceneBrowser() {
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    float menuBarHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(windowSize.x * 0.7f, menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(windowSize.x * 0.3f, windowSize.y - menuBarHeight));
    ImGui::Begin("Scene Browser", NULL, ImGuiWindowFlags_NoCollapse);

    const char* PlayText = editorState == EditorState::Playing ? "Pause" : "Play";
    if (ImGui::Button(PlayText)) {
        editorState = editorState == EditorState::Playing ? EditorState::Paused : EditorState::Playing;
        if (editorState != EditorState::Paused) {
            currentState = GameState::GameMode;
            scene->start();
            scene->Save("scene.json");
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        editorState = EditorState::Paused;
        currentState = GameState::EditorMode;
        scene->Load("scene.json");
    }

    if (ImGui::TreeNode("GameObjects")) {
        ImGui::Text("Objects");

        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            if (hitObject) {
                scene->GameObjects.erase(hitObject->name);
                delete hitObject;
                hitObject = nullptr;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Add")) {
            bShowNameInput = true;
        }

        ImGui::Separator();
        for (auto& obj : scene->GameObjects) {
            if (ImGui::Selectable(obj.second->name.c_str(), hitObject == obj.second)) {
                hitObject = obj.second;
            }
        }
        ImGui::TreePop();
    }

    if (bShowNameInput) {
        ImGui::OpenPopup("Add New Object");
        bShowNameInput = false;
    }

    if (ImGui::BeginPopupModal("Add New Object", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        char newObjectName[256] = "";

        ImGui::Text("Enter the name of the new object:");
        ImGui::InputText("Name", newObjectName, IM_ARRAYSIZE(newObjectName));

        if (ImGui::Button("Add", ImVec2(120, 0))) {
            if (strlen(newObjectName) == 0) {
                snprintf(newObjectName, sizeof(newObjectName), "Object_%zu", scene->GameObjects.size() + 1);
                scene->AddGameObject(newObjectName);
                scene->Save("scene.json");
                hitObject = nullptr;
            }
            else if (scene->GameObjects.find(newObjectName) != scene->GameObjects.end()) {
                ImGui::Text("Name already exists!");
                hitObject = nullptr;
            }
            else {
                scene->AddGameObject(newObjectName);
                hitObject = nullptr;
            }

            memset(newObjectName, 0, sizeof(newObjectName));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            memset(newObjectName, 0, sizeof(newObjectName));
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Dummy(ImVec2(0.0f, windowSize.y / 2));

    ImGui::Separator();
    ImGui::Text("Properties Panel");

    if (hitObject != nullptr) {
        ImGui::Text("Name: %s", hitObject->name.c_str());
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", hitObject->Position.x, hitObject->Position.y, hitObject->Position.z);

        ImGui::Text("Edit Position:");
        ImGui::SameLine();
        ImGui::DragFloat3("##Position", &hitObject->Position.x);

        ImGui::Text("Edit Rotation:");
        ImGui::SameLine();
        ImGui::DragFloat3("##Rotation", &hitObject->Rotation.x);

        ImGui::Text("Edit Scale:");
        ImGui::SameLine();
        ImGui::DragFloat3("##Scale", &hitObject->Size.x);

        ImGui::Checkbox("Visible", &hitObject->IsVisible);

        if (!hitObject->Components.empty()) {
            if (ImGui::CollapsingHeader("Components")) {
                for (auto it = hitObject->Components.begin(); it != hitObject->Components.end();) {
                    auto& comp = *it;
                    if (ImGui::TreeNode(comp->GetName())) {
                        comp->RenderUI();
                        scene->Save("scene.json");
                        if (ImGui::Button("Remove Component")) {
                            it = hitObject->Components.erase(it);
                        }
                        else {
                            ++it;
                        }
                        ImGui::TreePop();
                    }
                    else {
                        ++it;
                    }
                }
            }
        }

        ShowComponentAdderUI();
    }
    else {
        ImGui::Text("No object selected.");
    }
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        bBlockedInput = true;
    }
    ImGui::End();
}

bool CaseInsensitiveFind(const std::string& str, const std::string& substr) {
    std::string strLower = str;
    std::string substrLower = substr;
    std::transform(strLower.begin(), strLower.end(), strLower.begin(), ::tolower);
    std::transform(substrLower.begin(), substrLower.end(), substrLower.begin(), ::tolower);

    return strLower.find(substrLower) != std::string::npos;
}


void BDEngine::ShowComponentAdderUI()
{
    static char searchQuery[128] = "";
    ImGui::InputText("Search", searchQuery, IM_ARRAYSIZE(searchQuery));
    std::string query = searchQuery;
    std::transform(query.begin(), query.end(), query.begin(), ::tolower);

    if (!query.empty()) {
        for (const auto& category : componentCategories) {
            bool hasVisibleComponents = false;

            for (const auto& component : category.second) {
                if (searchQuery[0] == '\0' || CaseInsensitiveFind(component, searchQuery)) {
                    hasVisibleComponents = true;
                    break;
                }
            }

            if (!hasVisibleComponents) continue;

            if (ImGui::CollapsingHeader(category.first.c_str())) {
                for (const auto& component : category.second) {
                    if (searchQuery[0] == '\0' || CaseInsensitiveFind(component, searchQuery)) {
                        if (ImGui::Selectable(component.c_str())) {
                            hitObject->AddComponentByNameToObject(component);
                            for (auto comp : hitObject->Components)
                                comp->SetOwner(hitObject);
                            scene->Save("scene.json");
                        }
                    }
                }
            }
        }
    }
}
