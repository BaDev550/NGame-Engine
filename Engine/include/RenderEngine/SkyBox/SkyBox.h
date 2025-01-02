#ifndef SKYBOX_H
#define SKYBOX_H
#pragma once

#include <ResourceManager.h>
#include <vector>
#include <string>

class SkyBox
{
public:
    SkyBox();
    ~SkyBox();

    unsigned int cubemapTexture;
private:
    unsigned int skyboxVAO;
    std::vector<std::string> faces
    {
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    };

    unsigned int loadCubemap(std::vector<std::string> faces);
public:
    void Draw();
};

#endif // !SKYBOX_H

