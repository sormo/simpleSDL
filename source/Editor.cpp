#include "Editor.h"
#include "glm/gtc/matrix_transform.hpp"

static const glm::vec3 EDIT_COLOR(0.0f, 1.0f, 0.0f);
static const glm::vec3 ADD_COLOR(1.0f, 1.0f, 1.0f);

Editor::Editor(Scene & scene, UserInterface & userInterface, Camera & camera)
    : m_scene(scene), m_gui(userInterface), m_camera(camera), m_gizmo(scene), m_debug(*this)
{
    m_gui.shapeEditTypeClicked = [this]()
    {
        // remove whole body
        if (m_editShape)
            m_scene.RemoveBody(m_editShape->GetBody());
        m_editShape = nullptr;
        UpdateGizmo();

        if (m_gui.shapeEditType != UserInterface::ShapeEditType::None)
        {
            AddEditShape(EDIT_COLOR);
        }
        else
        {
            // edit has been turned off, reset edit shape data
            m_gui.shapePosition = Common::GetPointWorldSpace(glm::vec2(Common::GetWindowWidth() / 2.0f, Common::GetWindowHeight() / 2.0f), 10.0f, m_camera);
            m_gui.shapeScale = glm::vec3(1.0f, 1.0f, 1.0f);
            m_gui.shapeRotation = glm::vec3(0.0f, 0.0f, 0.0f);
            m_gui.isStatic = true;
        }
    };

    m_gui.shapeAcceptClicked = [this]()
    {
        m_scene.RemoveBody(m_editShape->GetBody());

        AddBodyToScene(ADD_COLOR);
        m_editShape = nullptr;
        UpdateGizmo();

        // translate shape a little and add it again as edit shape
        m_gui.shapePosition += glm::vec3(0.3f, 0.0f, 0.0f);
        m_gui.shapeEditTypeClicked();
    };

    m_gui.shapeEditModeClicked = [this]()
    {
        UpdateGizmo();
    };

    m_gui.shapeCameraClicked = [this]()
    {
        m_camera.SetLookPoint(m_gui.shapePosition);
    };
}

bool Editor::Press(const glm::vec2 & position, int64_t id)
{
    m_cursorPosition = position;
    m_pressPosition = position;

    Common::Math::Line ray = Common::GetRay(position, m_camera);
    m_gizmo.UpdateSelectedAxis(m_scene.RayCast(m_camera.GetPosition(), ray.vector), m_camera.GetPosition());

    if (m_gui.shapeEditType == UserInterface::ShapeEditType::None || m_gizmo.GetSelectedAxis() == Gizmo::Axis::None)
        return false;

    // gizmo is pressed, store press offset
    ComputeGizmoOffset(position);

    return true;
}

bool Editor::Release(const glm::vec2 & position, int64_t id)
{
    static const float MINIMUM_MOVE_FOR_CLICK = 2.0f;

    m_gizmo.UpdateSelectedAxis({}, m_camera.GetPosition());

    // if there is no move, cast a ray to select new editor object
    if (glm::length(position - m_pressPosition) < MINIMUM_MOVE_FOR_CLICK)
    {
        Clicked(position);
    }

    return false;
}

bool Editor::Move(const glm::vec2 & position, int64_t id)
{
    if (m_gui.shapeEditType == UserInterface::ShapeEditType::None || m_gizmo.GetSelectedAxis() == Gizmo::Axis::None)
        return false;

    if (m_gui.shapeEditMode == UserInterface::ShapeEditMode::Translate)
        TranslateShape(position);
    else if (m_gui.shapeEditMode == UserInterface::ShapeEditMode::Rotate)
        RotateShape(position);
    else if (m_gui.shapeEditMode == UserInterface::ShapeEditMode::Scale)
        ScaleShape(position);

    m_cursorPosition = position;

    ResetEditShape();

    return true;
}

void Editor::Clicked(const glm::vec2 & position)
{
    Common::Math::Line ray = Common::GetRay(position, m_camera);
    glm::vec3 cameraPosition = m_camera.GetPosition();

    auto rayCastResult = m_scene.RayCast(cameraPosition, ray.vector);

    //m_debug.Ray(cameraPosition, ray.vector, rayCastResult);

    // remove gizmo shapes
    m_gizmo.FilterGizmoShapes(rayCastResult);

    if (!rayCastResult.empty())
    {
        // pick the closest
        float minDistance = FLT_MAX;
        Scene::Shape closestShape = nullptr;

        for (auto[shape, position] : rayCastResult)
        {
            float shapeDistance = glm::distance(m_camera.GetPosition(), position);
            if (shapeDistance < minDistance)
            {
                minDistance = shapeDistance;
                closestShape = shape;
            }
        }

        SetEditShape(closestShape);
    }
}

Common::Math::Plane Editor::GetRotatePlane()
{
    glm::vec3 a(1.0f, 0.0f, 0.0f), b(0.0f, 0.0f, 1.0f);

    switch (m_gizmo.GetSelectedAxis())
    {
    case Gizmo::Axis::X: // YZ plane
        a = glm::vec3(0.0f, 1.0f, 0.0f);
        b = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    case Gizmo::Axis::Z: // XY plane
        a = glm::vec3(1.0f, 0.0f, 0.0f);
        b = glm::vec3(0.0f, 1.0f, 0.0f);
        break;
    case Gizmo::Axis::Y: // XZ plane
        a = glm::vec3(1.0f, 0.0f, 0.0f);
        b = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    }

    return Common::Math::Plane::CreateFromPoints({ 0.0f, 0.0f, 0.0f }, a, b);
}

Common::Math::Plane Editor::GetEditPlane()
{
    const glm::vec3& c = m_camera.GetPosition();
    Common::Math::Plane result;

    result.value = 0.0f;

    switch (m_gizmo.GetSelectedAxis())
    {
    case Gizmo::Axis::X:
        result.normal = glm::normalize(glm::vec3{ 0.0f, c.y, c.z });
        break;
    case Gizmo::Axis::Z:
        result.normal = glm::normalize(glm::vec3{ c.x, c.y, 0.0f });
        break;
    case Gizmo::Axis::Y:
        result.normal = glm::normalize(glm::vec3{ c.x, 0.0f, c.z });
        break;
    }

    return result;
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

    glm::vec3 scaleVector(p2 - p1);

    //m_gui.shapeScale += RestrictToLine(scaleVector);

    auto editVector = Common::Math::RotateVector(GetEditLineUnit(), m_gui.shapeRotation);
    float scaleValue = glm::dot(editVector, scaleVector);

    scaleValue *= 2.0f;

    ModifyScaleValue(scaleValue);

    static const float MINIMUM_SCALE = 0.1f;
    m_gui.shapeScale.x = m_gui.shapeScale.x < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.shapeScale.x;
    m_gui.shapeScale.y = m_gui.shapeScale.y < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.shapeScale.y;
    m_gui.shapeScale.z = m_gui.shapeScale.z < MINIMUM_SCALE ? MINIMUM_SCALE : m_gui.shapeScale.z;

    //m_debug.EditPlane(plane, m_editShape->GetBody()->GetPosition());
    //m_debug.CurrentAxis(editVector, m_editShape->GetBody()->GetPosition());
}

void Editor::ModifyScaleValue(float scaleValue)
{
    if (m_gui.shapeEditType == UserInterface::ShapeEditType::Cube)
    {
        switch (m_gizmo.GetSelectedAxis())
        {
        case Gizmo::Axis::X:
            m_gui.shapeScale.x += scaleValue; break;
        case Gizmo::Axis::Y:
            m_gui.shapeScale.y += scaleValue; break;
        case Gizmo::Axis::Z:
            m_gui.shapeScale.z += scaleValue; break;
        }
    }
    else if (m_gui.shapeEditType == UserInterface::ShapeEditType::Sphere)
    {
        switch (m_gizmo.GetSelectedAxis())
        {
        case Gizmo::Axis::X:
        case Gizmo::Axis::Y:
        case Gizmo::Axis::Z:
            m_gui.shapeScale.z += scaleValue; break;
        }
        m_gui.shapeScale.x += scaleValue;
    }
    else if (m_gui.shapeEditType == UserInterface::ShapeEditType::Cylinder ||
             m_gui.shapeEditType == UserInterface::ShapeEditType::Cone)
    {
        switch (m_gizmo.GetSelectedAxis())
        {
        case Gizmo::Axis::X:
        case Gizmo::Axis::Z:
            m_gui.shapeScale.x += scaleValue; break;
        case Gizmo::Axis::Y:
            m_gui.shapeScale.y += scaleValue; break;
        }
    }
}

void Editor::RotateShape(const glm::vec2 & position)
{
    Common::Math::Plane plane = GetRotatePlane();

    auto bodyPosition = m_editShape->GetBody()->GetPosition();

    plane.Translate(m_gui.shapePosition);
    plane.Rotate(m_gui.shapeRotation);
    plane.Position(bodyPosition);

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

    float angle = Common::Math::GetAngle(p1 - bodyPosition, p2 - bodyPosition, plane);

    // invert angle if plane is rotated on the other side
    //if (Common::Math::GetDistance(plane, m_camera.GetPosition()) < 0.0f)
    //    angle = -angle;

    glm::vec3 axis = glm::normalize(plane.normal);

    m_gui.shapeRotation = Common::Math::RotateRotation(m_gui.shapeRotation, angle, axis);

    //m_debug.EditPlane(plane, bodyPosition);
    //m_debug.RotationAxis(axis, bodyPosition);
    //m_debug.CurrentAxis(currentAxis, bodyPosition);
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

    glm::vec3 planeIntersection = Common::Math::GetIntersection(plane, ray);

    m_gui.shapePosition += RestrictToLine(planeIntersection - m_gui.shapePosition) - RestrictToLine(m_gizmoOffset);

    //m_debug.EditPlane(plane, m_gui.shapePosition);
}

glm::vec3 Editor::RestrictToLine(const glm::vec3& point)
{
    return point * GetEditLineUnit();
}

glm::vec3 Editor::GetEditLineUnit()
{
    switch (m_gizmo.GetSelectedAxis())
    {
    case Gizmo::Axis::X:
        return glm::vec3(1.0f, 0.0f, 0.0f);
    case Gizmo::Axis::Y:
        return glm::vec3(0.0f, 1.0f, 0.0f);
    case Gizmo::Axis::Z:
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }
    return glm::vec3(1.0f, 0.0f, 0.0f);
}

Scene::Body Editor::AddBodyToScene(const glm::vec3& color)
{
    Material::Data material;
    material.ambient = material.diffuse = material.specular = color;
    material.shininess = 10.0f;

    bool isEdit = color == EDIT_COLOR;
    bool isStatic = isEdit ? true : m_gui.isStatic;

    Scene::Body newBody = nullptr;

    if (m_gui.shapeEditType == UserInterface::ShapeEditType::Cube)
        newBody = m_scene.AddCube({ m_gui.shapePosition, m_gui.shapeRotation, m_gui.shapeScale }, material, isStatic);
    else if (m_gui.shapeEditType == UserInterface::ShapeEditType::Sphere)
        newBody = m_scene.AddSphere({ m_gui.shapePosition, m_gui.shapeRotation, m_gui.shapeScale.x / 2.0f }, material, isStatic);
    else if (m_gui.shapeEditType == UserInterface::ShapeEditType::Cylinder)
        newBody = m_scene.AddCylinder({ m_gui.shapePosition, m_gui.shapeRotation, m_gui.shapeScale.x / 2.0f, m_gui.shapeScale.y }, material, isStatic);
    else if (m_gui.shapeEditType == UserInterface::ShapeEditType::Cone)
        newBody = m_scene.AddCone({ m_gui.shapePosition, m_gui.shapeRotation, m_gui.shapeScale.x / 2.0f, m_gui.shapeScale.y }, material, isStatic);

    return newBody;
}

void Editor::AddEditShape(const glm::vec3 & color)
{
    Scene::Body body = AddBodyToScene(color);

    // just pick first shape
    m_editShape = body->GetShapes()[0];

    UpdateGizmo();
}

void Editor::SetEditShape(Scene::Shape shape)
{
    if (m_editShape)
    {
        m_scene.RemoveBody(m_editShape->GetBody());
        AddBodyToScene(ADD_COLOR);
    }

    // un-select
    if (m_editShape == shape)
    {
        m_editShape = nullptr;
        UpdateGizmo();
        m_gui.shapeEditType = UserInterface::ShapeEditType::None;

        return;
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
    m_editShape = nullptr;

    AddEditShape(EDIT_COLOR);
}

void Editor::UpdateGizmo()
{
    if (!m_editShape)
    {
        m_gizmo.SetMode(Gizmo::Mode::None);
        return;
    }

    switch (m_gui.shapeEditMode)
    {
    case UserInterface::ShapeEditMode::Translate:
        m_gizmo.SetMode(Gizmo::Mode::Move);
        break;
    case UserInterface::ShapeEditMode::Rotate:
        m_gizmo.SetMode(Gizmo::Mode::Rotate);
        break;
    case UserInterface::ShapeEditMode::Scale:
        m_gizmo.SetMode(Gizmo::Mode::Scale);
        break;
    }

    // currently expect only single shape bodies
    m_gizmo.UpdateBody(m_editShape->GetBody());
}

void Editor::ComputeGizmoOffset(const glm::vec2& position)
{
    Common::Math::Line ray = Common::GetRay(position, m_camera);

    Common::Math::Plane plane = GetEditPlane();
    plane.Translate(m_gui.shapePosition);

    glm::vec3 planeIntersection = Common::Math::GetIntersection(plane, ray);

    m_gizmoOffset = planeIntersection - m_editShape->GetBody()->GetPosition();
}

void Editor::Draw(const glm::mat4& view, const glm::mat4& projection)
{
    m_gizmo.Draw(view, projection);

    m_debug.Draw(view, projection);
    m_debug.Clear();
}
