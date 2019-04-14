#include "Editor.h"
#include "glm/gtc/matrix_transform.hpp"

static const glm::vec3 EDIT_COLOR(0.0f, 1.0f, 0.0f);
static const glm::vec3 ADD_COLOR(1.0f, 1.0f, 1.0f);

Editor::Editor(Scene & scene, UserInterface & userInterface, Camera & camera)
    : m_scene(scene), m_gui(userInterface), m_camera(camera)
{
    m_gui.shapeEditClicked = [this]()
    {
        m_gui.shapePosition = Common::GetPointWorldSpace(glm::vec2(Common::GetWindowWidth() / 2.0f, Common::GetWindowHeight() / 2.0f), 10.0f, m_camera);
        m_gui.shapeScale = glm::vec3(1.0f, 1.0f, 1.0f);
        m_gui.shapeRotation = glm::vec3(0.0f, 0.0f, 0.0f);
        m_gui.isStatic = true;

        // remove whole body
        if (m_editShape)
            m_scene.RemoveBody(m_editShape->GetBody());
        m_editShape = nullptr;

        if (m_gui.shapeEditType != UserInterface::ShapeEditType::None)
            AddEditShape(EDIT_COLOR);
    };

    m_gui.shapeAcceptClicked = [this]()
    {
        m_scene.RemoveBody(m_editShape->GetBody());

        AddEditShape(ADD_COLOR);
        m_editShape = nullptr;

        m_gui.shapeEditClicked();
    };

    CreateCoordinateSystem();
}

bool Editor::Press(const glm::vec2 & position, int64_t id)
{
    m_isPressed = true;
    m_cursorPosition = position;
    m_pressPosition = position;

    if (m_gui.shapeEditType == UserInterface::ShapeEditType::None || m_gui.shapeEditMode == UserInterface::ShapeEditMode::Camera)
        return false;

    if (m_gui.shapeEditMode == UserInterface::ShapeEditMode::Translate)
    {
        TranslateShape(position);
        ResetEditShape();
    }

    return true;
}

bool Editor::Release(const glm::vec2 & position, int64_t id)
{
    static const float MINIMUM_MOVE_FOR_CLICK = 2.0f;

    m_isPressed = false;

    // if there is no move, cast a ray to select new editor object
    if (glm::length(position - m_pressPosition) < MINIMUM_MOVE_FOR_CLICK)
    {
        Clicked(position);
    }

    return false;
}

bool Editor::Move(const glm::vec2 & position, int64_t id)
{
    if (m_gui.shapeEditType == UserInterface::ShapeEditType::None || !m_isPressed || m_gui.shapeEditMode == UserInterface::ShapeEditMode::Camera)
        return false;

    if (m_gui.shapeEditMode == UserInterface::ShapeEditMode::Translate)
        TranslateShape(position);
    else if (m_gui.shapeEditMode == UserInterface::ShapeEditMode::Rotate)
        RotateShape(position);
    else if (m_gui.shapeEditMode == UserInterface::ShapeEditMode::Scale)
        ScaleShape(position);

    ResetEditShape();

    return true;
}

void Editor::Clicked(const glm::vec2 & position)
{
    Common::Math::Line ray = Common::GetRay(position, m_camera);

    auto rayCastResult = m_scene.RayCast(m_camera.GetPosition(), ray.vector);

    if (!rayCastResult.empty())
    {
        // pick the closest

        float minDistance = FLT_MAX;
        Scene::Shape shape = nullptr;

        for (auto obj : rayCastResult)
        {
            float objDistance = glm::distance(m_camera.GetPosition(), obj->GetPosition());
            if (objDistance < minDistance)
            {
                minDistance = objDistance;
                shape = obj;
            }
        }

        SetEditShape(shape);
    }
}

Common::Math::Plane Editor::GetEditPlane()
{
    glm::vec3 a, b;

    switch (m_gui.shapeEditAxis)
    {
    case UserInterface::ShapeEditAxis::XY:
        a = glm::vec3(1.0f, 0.0f, 0.0f);
        b = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case UserInterface::ShapeEditAxis::XZ:
        a = glm::vec3(1.0f, 0.0f, 0.0f);
        b = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    case UserInterface::ShapeEditAxis::YZ:
        a = glm::vec3(0.0f, 1.0f, 0.0f);
        b = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    }

    return Common::Math::Plane::CreateFromPoints({0.0f, 0.0f, 0.0f}, a, b);
}

void Editor::ScaleShape(const glm::vec2 & position)
{
    Common::Math::Plane plane = GetEditPlane();

    plane.Translate(m_gui.shapePosition);
    plane.Rotate(m_gui.shapeRotation);

    glm::vec3 p1;
    {
        Common::Math::Line ray = Common::GetRay(m_cursorPosition, m_camera);
        p1 = Common::Math::GetIntersection(plane, ray);
    }
    glm::vec3 p2;
    {
        Common::Math::Line ray = Common::GetRay(position, m_camera);
        p2 = Common::Math::GetIntersection(plane, ray);
    }

    glm::vec3 scaleVector(p1 - p2);

    m_gui.shapeScale += scaleVector;
    
    static const float MINIMUM_SCALE = 0.1f;
    m_gui.shapeScale.x = m_gui.shapeScale.x < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.shapeScale.x;
    m_gui.shapeScale.y = m_gui.shapeScale.y < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.shapeScale.y;
    m_gui.shapeScale.z = m_gui.shapeScale.z < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.shapeScale.z;

    m_cursorPosition = position;
}

void Editor::RotateShape(const glm::vec2 & position)
{
    glm::vec2 delta = position - m_cursorPosition;
    m_cursorPosition = position;

    m_gui.shapeRotation.y += glm::radians(delta.x);
    m_gui.shapeRotation.x += glm::radians(delta.y);
}

void Editor::TranslateShape(const glm::vec2 & position)
{
    // ***********************************************************************
    // // get point on line A and vector of line v
    // glm::vec3 A(m_camera.GetPosition());
    // glm::vec3 v = Common::GetPointWorldSpace(position, 10.0f, m_camera) - A;
    // // compute parameter in parametric equation of line according to constant axis
    // float t = (m_gui.cubePosition.z - A.z) / v.z;
    // glm::vec3 result;
    // result.x = A.x + v.x * t;
    // result.y = A.y + v.y * t;
    // result.z = m_gui.cubePosition.z;
    // ***********************************************************************

    Common::Math::Line ray = Common::GetRay(position, m_camera);
    Common::Math::Plane plane = GetEditPlane();

    plane.Translate(m_gui.shapePosition);

    m_gui.shapePosition = Common::Math::GetIntersection(plane, ray);
}

void Editor::AddEditShape(const glm::vec3 & color)
{
    Material::Data material;
    material.ambient = material.diffuse = material.specular = color;
    material.shininess = 10.0f;

    bool isEdit = color == EDIT_COLOR;
    bool isStatic = isEdit ? true : m_gui.isStatic;

    Scene::Body newBody;

    if (m_gui.shapeEditType == UserInterface::ShapeEditType::Cube)
        newBody = m_scene.AddCube({ m_gui.shapePosition, m_gui.shapeRotation, m_gui.shapeScale }, material, isStatic);
    else if (m_gui.shapeEditType == UserInterface::ShapeEditType::Sphere)
        newBody = m_scene.AddSphere({ m_gui.shapePosition, m_gui.shapeRotation, m_gui.shapeScale.x / 2.0f }, material, isStatic);
    else if (m_gui.shapeEditType == UserInterface::ShapeEditType::Cylinder)
        newBody = m_scene.AddCylinder({ m_gui.shapePosition, m_gui.shapeRotation, m_gui.shapeScale.x / 2.0f, m_gui.shapeScale.y }, material, isStatic);
    else if (m_gui.shapeEditType == UserInterface::ShapeEditType::Cone)
        newBody = m_scene.AddCone({ m_gui.shapePosition, m_gui.shapeRotation, m_gui.shapeScale.x / 2.0f, m_gui.shapeScale.y }, material, isStatic);

    // just pick first shape
    m_editShape = newBody->GetShapes()[0];
}

void Editor::SetEditShape(Scene::Shape shape)
{
    if (m_editShape && m_editShape != shape)
    {
        m_scene.RemoveBody(m_editShape->GetBody());
        AddEditShape(ADD_COLOR);
    }

    m_editShape = shape;

    m_gui.shapePosition = shape->GetBody()->GetPosition();
    m_gui.shapeRotation = shape->GetBody()->GetRotation();
    m_gui.shapeScale = shape->GetScale();
    // TODO check this scaling
    if (shape->GetType() != Shapes::Type::Cube)
    {
        m_gui.shapeScale.x *= 2.0f;
        //m_gui.shapeScale.y *= 2.0f;
    }
    m_gui.isStatic = shape->GetBody()->IsStatic();

    switch (shape->GetType())
    {
    case Shapes::Type::Cone: m_gui.shapeEditType = UserInterface::ShapeEditType::Cone; break;
    case Shapes::Type::Cube: m_gui.shapeEditType = UserInterface::ShapeEditType::Cube; break;
    case Shapes::Type::Cylinder: m_gui.shapeEditType = UserInterface::ShapeEditType::Cylinder; break;
    case Shapes::Type::Sphere: m_gui.shapeEditType = UserInterface::ShapeEditType::Sphere; break;
    }

    ResetEditShape();
}

void Editor::ResetEditShape()
{
    m_scene.RemoveBody(m_editShape->GetBody());

    AddEditShape(EDIT_COLOR);
}

void Editor::CreateCoordinateSystem()
{
    Material::Data red{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0.0f };
    Material::Data green{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 0.0f };
    Material::Data blue{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f };

    std::vector<std::tuple<Shapes::Defintion::Cylinder, Material::Data>> axes;
    std::vector<std::tuple<Shapes::Defintion::Cone, Material::Data>> arrows;

    axes.push_back({ { { 0.0f, 0.5f, 0.0f }, glm::vec3(0.0f), 0.1f, 1.0f }, red });
    axes.push_back({ { { 0.0f, 0.0f, 0.5f }, Common::Math::GetRotation(glm::radians(90.0f), { 1.0f, 0.0f, 0.0f }), 0.1f, 1.0f }, green });
    axes.push_back({ { { 0.5f, 0.0f, 0.0f }, Common::Math::GetRotation(-glm::radians(90.0f), { 0.0f, 0.0f, 1.0f }), 0.1f, 1.0f }, blue });

    arrows.push_back({ { { 0.0f, 1.1f, 0.0f }, glm::vec3(0.0f), 0.2f, 0.2f }, red });
    arrows.push_back({ { { 0.0f, 0.0f, 1.1f }, Common::Math::GetRotation(glm::radians(90.0f), { 1.0f, 0.0f, 0.0f }), 0.2f, 0.2f }, green });
    arrows.push_back({ { { 1.1f, 0.0f, 0.0f }, Common::Math::GetRotation(-glm::radians(90.0f), { 0.0f, 0.0f, 1.0f }), 0.2f, 0.2f }, blue });

    m_coordinateSystem = m_scene.AddCompound({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, true, {}, {}, axes, arrows);
}
