#include "Generator.h"
#include "Common.h"
#include "scene/Scene.h"
#include <memory>

static const char* GENERATORS[] = { "Empty", "Boxes", "Moon-Earth" };

namespace Application
{
    extern std::unique_ptr<Scene> g_scene;
}

namespace Generator
{
    const char* const* GetNames()
    {
        return GENERATORS;
    }

    size_t GetCount()
    {
        return COUNTOF(GENERATORS);
    }

    void GenerateBoxes()
    {
        Material::Data material;
        material.ambient = material.diffuse = material.specular = glm::vec3(1.0, 1.0, 1.0);
        material.shininess = 15.0f;

        Application::g_scene->AddCube({ glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(20.0f, 0.5f, 20.0f) }, material, true);
        Application::g_scene->AddCube({ glm::vec3(0.0f, 1.5f, 0.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f) }, material, true);
        Application::g_scene->AddCube({ glm::vec3(2.0f, 0.0f, 1.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f) }, material, true);
        Application::g_scene->AddCube({ glm::vec3(-1.0f, 0.0f, 2.0), Common::Math::GetRotation(glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))), glm::vec3(0.25) }, material, true);
    }

    void GenerateEarthMoon()
    {
        Material::Data material;
        material.ambient = material.diffuse = material.specular = glm::vec3(1.0, 1.0, 1.0);

        Shapes::Defintion::Sphere moon;

        moon.position = glm::vec3(0.0f, 384.4f, 0.0f);
        moon.radius = 1.7371f;

        Application::g_scene->AddSphere(moon, material, true);

        Shapes::Defintion::Sphere earth;

        earth.position = glm::vec3(0.0f, 0.0f, 0.0f);
        earth.radius = 6.371f;

        Application::g_scene->AddSphere(earth, material, true);
    }

    void Changed(int32_t index)
    {
        Application::g_scene->Clear();

        switch (index)
        {
        case 0:
            break;
        case 1:
            GenerateBoxes();
            break;
        case 2:
            GenerateEarthMoon();
            break;
        }
    }
}
