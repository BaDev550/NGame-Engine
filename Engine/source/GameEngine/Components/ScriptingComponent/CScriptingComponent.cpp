#include <GameEngine/Components/ScriptingComponent/CScriptingComponent.h>

bool showScriptFileSelector;

void CScriptingComponent::ShowFileBrowser()
{
    if (!showScriptFileSelector) return;

    ImGui::Begin("File Browser");

    ImGui::Text("Current Path: %s", currentPath.c_str());
    ImGui::Separator();

    if (ImGui::Button("Up")) {
        if (std::filesystem::path(currentPath).has_parent_path()) {
            currentPath = std::filesystem::path(currentPath).parent_path().string();
        }
    }

    for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
        const std::string name = entry.path().filename().string();
        const bool isDirectory = entry.is_directory();

        if (isDirectory) {
            if (ImGui::Selectable((name + "/").c_str())) {
                currentPath = entry.path().string();
            }
        }
        else {
            if (ImGui::Selectable(name.c_str())) {
                selectedFile = entry.path().string();
                ScriptName = entry.path().stem().string();
            }
        }
    }

    if (!selectedFile.empty()) {
        ImGui::Separator();
        ImGui::Text("Selected File: %s", selectedFile.c_str());

        if (ImGui::Button("Load Script")) {
            ScriptPath = selectedFile;
            selectedFile.clear();
        }
    }
    ImGui::End();

    ImGui::Separator();
    ImGui::Text("Loaded File: %s", ScriptPath.c_str());
    ImGui::Text("Script Name: %s", ScriptName.c_str());
}

std::string CScriptingComponent::TranslateCppScript(const std::string& cppContent)
{
    std::string translatedContent = cppContent;

    std::regex getComponentRegex(R"(GetComponent<([a-zA-Z_][a-zA-Z0-9_]*)>\(\))");
    translatedContent = std::regex_replace(translatedContent, getComponentRegex, "FindComponent<$1>(this)");

    return translatedContent;
}

void CScriptingComponent::CompileAndExecuteCppScript(const std::string& cppCode)
{
    std::ifstream scriptFile(ScriptPath);
    if (!scriptFile.is_open()) {
        std::cerr << "C++ script dosyasý açýlamadý: " << ScriptPath << std::endl;
        return;
    }

    std::string scriptContent((std::istreambuf_iterator<char>(scriptFile)),
        std::istreambuf_iterator<char>());
    scriptFile.close();

    std::ofstream tempScriptFile("temp_script.cpp");
    if (!tempScriptFile.is_open()) {
        std::cerr << "Geçici dosya oluþturulamadý!" << std::endl;
        return;
    }
    tempScriptFile << scriptContent;
    tempScriptFile.close();

    std::string command = "g++ -o temp_script temp_script.cpp "
        "-I.E:/CPP_Projects/Minecraft/Engine "
        "-I.E:/CPP_Projects/Minecraft/Linking/include "
        "-I.E:/CPP_Projects/Minecraft/Linking/include "
        "-L.E:/CPP_Projects/Minecraft/Linking/lib -lGLFW -lGL -lstdc++";

    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Derleme baþarýsýz!" << std::endl;
        return;
    }

    command = "./temp_script";
    result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Çalýþtýrma baþarýsýz!" << std::endl;
        return;
    }

    std::cout << "Script baþarýyla çalýþtýrýldý!" << std::endl;
}

void CScriptingComponent::ParseAndRunScript(const std::string& scriptContent) {
    if (scriptContent.find("ApplyForce") != std::string::npos) {
        methodMap["ApplyForce"]();
    }
}

void CScriptingComponent::ExecuteFunction(const std::string& functionName) {
    if (methodMap.find(functionName) != methodMap.end()) {
        methodMap[functionName]();
    }
    else {
        std::cerr << "Function " << functionName << " not found!" << std::endl;
    }
}

void CScriptingComponent::RenderUI()
{
    if (ImGui::Button("Load Script")) {
        showScriptFileSelector = showScriptFileSelector ? false : true;
    }

    ShowFileBrowser();
}
