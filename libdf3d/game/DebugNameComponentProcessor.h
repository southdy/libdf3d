#pragma once

#include <game/Entity.h>
#include <game/EntityComponentProcessor.h>

namespace df3d {

class DF3D_DLL DebugNameComponentProcessor : public EntityComponentProcessor
{
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    void update() override;
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    const std::string& getName(Entity e);
    Entity getByName(const std::string &name);

    void add(Entity e, const std::string &name);
    void remove(Entity e);
};

}