#pragma once

#include <df3d/game/Entity.h>
#include <df3d/game/EntityComponentProcessor.h>

namespace df3d {

class AudioBuffer;
class World;

class DF3D_DLL AudioComponentProcessor : public EntityComponentProcessor
{
public:
    enum class State
    {
        INITIAL,
        PLAYING,
        PAUSED,
        STOPPED
    };

private:
    struct Impl;
    unique_ptr<Impl> m_pimpl;

    World *m_world;

    void streamThread();

    void update() override;
    void cleanStep(const std::list<Entity> &deleted) override;

public:
    AudioComponentProcessor(World *world);
    ~AudioComponentProcessor();

    void play(Entity e);
    void stop(Entity e);
    void pause(Entity e);

    void setSoundVolume(float volume);

    void setListenerPosition(const glm::vec3 &pos);
    void setListenerOrientation(const glm::vec3 &dir, const glm::vec3 &up);

    void setPitch(Entity e, float pitch);
    void setGain(Entity e, float gain);
    void setLooped(Entity e, bool looped);
    void setRolloffFactor(Entity e, float factor);

    float getPitch(Entity e) const;
    float getGain(Entity e) const;
    bool isLooped(Entity e) const;
    State getState(Entity e) const;

    void add(Entity e, const std::string &audioFilePath, bool streamed = false);
    void remove(Entity e);
    bool has(Entity e);
};

}