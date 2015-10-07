#include "PhysicsComponentSerializer.h"

#include <utils/JsonUtils.h>
#include <components/PhysicsComponent.h>

namespace df3d { namespace component_serializers {

Component PhysicsComponentSerializer::fromJson(const Json::Value &root)
{
    using namespace components;

    auto type = root["type"].asString();
    if (type != "StaticBody")
    {
        base::glog << "Unsupported rigid body type" << type << base::logwarn;
        return nullptr;
    }

    PhysicsComponent::CollisionShapeType colShapeType;
    PhysicsComponent::CreationParams params;

    auto shape = root["shape"].asString();
    if (shape == "box")
        colShapeType = PhysicsComponent::CollisionShapeType::BOX;
    else if (shape == "sphere")
        colShapeType = PhysicsComponent::CollisionShapeType::SPHERE;
    else if (shape == "convex_hull")
        colShapeType = PhysicsComponent::CollisionShapeType::CONVEX_HULL;
    else
    {
        base::glog << "Unsupported rigid body box shape" << shape << base::logwarn;
        return nullptr;
    }

    params.type = colShapeType;
    params.mass = utils::json::getOrDefault(root["mass"], params.mass);
    params.friction = utils::json::getOrDefault(root["friction"], params.friction);
    params.restitution = utils::json::getOrDefault(root["restitution"], params.restitution);
    params.linearDamping = utils::json::getOrDefault(root["linearDamping"], params.linearDamping);
    params.angularDamping = utils::json::getOrDefault(root["angularDamping"], params.angularDamping);
    params.group = utils::json::getOrDefault(root["group"], params.group);
    params.mask = utils::json::getOrDefault(root["mask"], params.mask);

    return make_shared<PhysicsComponent>(params);
}

Json::Value PhysicsComponentSerializer::toJson(Component component)
{
    // TODO:
    assert(false);

    Json::Value result;

    return result;
}

} }