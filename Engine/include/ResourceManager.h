#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#pragma once

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "RenderEngine/Shader/Shader.h"
#include "RenderEngine/Shader/Texture.h"

class ResourceManager
{
public:
    ResourceManager() {};
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;
    
    static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name);
    static Shader GetShader(std::string name);
    static Texture2D LoadTexture(const char* file, std::string name);
    static Texture2D LoadTextureDir(const char* path, const std::string& directory);
    static Texture2D& GetTexture(const std::string& name);
    static void Clear();
private:
    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile);
    static Texture2D loadTextureFromFile(const char* file);
    static Texture2D loadTextureFromDirectory(const char* path, const std::string& directory);
};

#endif
