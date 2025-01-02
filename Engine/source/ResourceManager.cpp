#include "ResourceManager.h"

#include "stb/stb_image.h"

std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, std::string name)
{
    Textures[name] = loadTextureFromFile(file);
    std::cout << "Texture loaded the name of: " << name << " from file: " << file << std::endl;
    return Textures[name];
}

Texture2D ResourceManager::LoadTextureDir(const char* path, const std::string& directory)
{
    Textures[path] = loadTextureFromDirectory(path, directory);
    std::cout << "Texture loaded the name of: " << path << " from file: " << directory << std::endl;
    return Textures[path];
}

Texture2D& ResourceManager::GetTexture(const std::string& name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile)
{
    std::string vertexCode;
    std::string fragmentCode;
    try
    {
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file)
{
    stbi_set_flip_vertically_on_load(true);
    Texture2D texture;
    texture.Filter_Min = GL_LINEAR_MIPMAP_LINEAR;
    texture.Filter_Max = GL_LINEAR;

    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

    if (nrChannels == 1) {
        texture.Internal_Format = GL_RED;
        texture.Image_Format = GL_RED;
    }
    else if (nrChannels == 3) {
        texture.Internal_Format = GL_RGB;
        texture.Image_Format = GL_RGB;
    }
    else if (nrChannels == 4) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    texture.Generate(width, height, data);

    stbi_image_free(data);
    return texture;
}

Texture2D ResourceManager::loadTextureFromDirectory(const char* path, const std::string& directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    Texture2D texture;

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        if (nrChannels == 1) {
            texture.Internal_Format = GL_RED;
            texture.Image_Format = GL_RED;
        }
        else if (nrChannels == 3) {
            texture.Internal_Format = GL_RGB;
            texture.Image_Format = GL_RGB;
        }
        else if (nrChannels == 4) {
            texture.Internal_Format = GL_RGBA;
            texture.Image_Format = GL_RGBA;
        }
        texture.Filter_Min = GL_LINEAR_MIPMAP_LINEAR;
        texture.Filter_Max = GL_LINEAR;
        texture.Wrap_S = GL_REPEAT;
        texture.Wrap_T = GL_REPEAT;

        texture.Generate(width, height, data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return texture;
}