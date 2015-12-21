#include "FPSCamera.h"

#include <base/EngineController.h>
#include <input/InputManager.h>
#include <input/InputEvents.h>

namespace df3d {

void FPSCamera::move(const glm::vec3 &vec)
{
    setPosition(getPosition() + vec);
}

void FPSCamera::onGameDeltaTime(float dt)
{
    if (!m_freeMove)
        return;

    if (svc().inputManager().getMouseButton(MouseButton::LEFT))
    {
        static float yaw, pitch;
        yaw += int(svc().inputManager().getMouseDelta().x) * m_damping;
        pitch += int(svc().inputManager().getMouseDelta().y) * m_damping;

        while (yaw > 360.0f)
            yaw -= 360.0f;

        while (yaw < -360.0f)
            yaw += 360.0f;

        if (pitch > 90.0f) pitch = 90.0f;
        if (pitch < -90.0f) pitch = -90.0f;

        setOrientation(glm::vec3(-pitch, -yaw, 0.0f));
    }

    float dv = dt * m_velocity;

    if (svc().inputManager().getKey(KeyCode::KEY_UP))
        move(getDir() * dv);
    if (svc().inputManager().getKey(KeyCode::KEY_DOWN))
        move(-getDir() * dv);
    if (svc().inputManager().getKey(KeyCode::KEY_LEFT))
        move(-getRight() * dv);
    if (svc().inputManager().getKey(KeyCode::KEY_RIGHT))
        move(getRight() * dv);
}

FPSCamera::FPSCamera(float velocity, bool freeMove, float damping)
    : m_freeMove(freeMove),
    m_velocity(velocity),
    m_damping(damping)
{
    svc().timeManager().registerTimeListener(this);
}

FPSCamera::~FPSCamera()
{
    svc().timeManager().unregisterTimeListener(this);
}

}
