#pragma once

FWD_MODULE_CLASS(render, RenderManager)
FWD_MODULE_CLASS(render, RenderStats)
FWD_MODULE_CLASS(render, Viewport)
FWD_MODULE_CLASS(scene, SceneManager)
FWD_MODULE_CLASS(resources, ResourceManager)
FWD_MODULE_CLASS(resources, FileSystem)
FWD_MODULE_CLASS(platform, AppEvent)
FWD_MODULE_CLASS(gui, GuiManager)
FWD_MODULE_CLASS(gui, DebugOverlayWindow)
FWD_MODULE_CLASS(scripting, ScriptManager)
FWD_MODULE_CLASS(physics, PhysicsManager)
FWD_MODULE_CLASS(audio, AudioManager)

#include "EngineInitParams.h"

namespace df3d { namespace base {

using ConsoleCommandHandler = std::function<std::string(const std::string &)>;

class DF3D_DLL EngineController : boost::noncopyable
{
    EngineController();
    ~EngineController();

    render::RenderManager *m_renderManager = nullptr;
    scene::SceneManager *m_sceneManager = nullptr;
    resources::ResourceManager *m_resourceManager = nullptr;
    resources::FileSystem *m_fileSystem = nullptr;
    gui::GuiManager *m_guiManager = nullptr;
    scripting::ScriptManager *m_scriptManager = nullptr;
    physics::PhysicsManager *m_physics = nullptr;
    audio::AudioManager *m_audioManager = nullptr;

    gui::DebugOverlayWindow *m_debugWindow = nullptr;

    std::unordered_map<std::string, ConsoleCommandHandler> m_consoleCommandsHandlers;
    void consoleCommandInvoked(const std::string &name, std::string &result);

    bool m_initialized = false;

    TimePoint m_timeStarted;
    float m_timeElapsed = 0;

public:
    static EngineController *getInstance();

    bool init(EngineInitParams params);
    void shutdown();

    void update(float dt);
    void postUpdate();
    void runFrame();

    float getElapsedTime() const { return m_timeElapsed; }
    const render::RenderStats &getLastRenderStats() const;

    void toggleDebugWindow();

    void registerConsoleCommandHandler(const char *commandName, ConsoleCommandHandler handler);
    void unregisterConsoleCommandHandler(const char *commandName);

    bool initialized() const { return m_initialized; }

    void dispatchAppEvent(const platform::AppEvent &event);
    
    const render::Viewport &getViewport() const;
    void setViewport(const render::Viewport &newvp);

    scene::SceneManager *getSceneManager() { return m_sceneManager; }
    resources::ResourceManager *getResourceManager() { return m_resourceManager; }
    resources::FileSystem *getFileSystem() { return m_fileSystem; }
    render::RenderManager *getRenderManager() { return m_renderManager; }
    gui::GuiManager *getGuiManager() { return m_guiManager; }
    scripting::ScriptManager *getScriptManager() { return m_scriptManager; }
    physics::PhysicsManager *getPhysicsManager() { return m_physics; }
    audio::AudioManager *getAudioManager() { return m_audioManager; }
};

} }

#define g_engineController df3d::base::EngineController::getInstance()

#define g_sceneManager df3d::base::EngineController::getInstance()->getSceneManager()
#define g_resourceManager df3d::base::EngineController::getInstance()->getResourceManager()
#define g_fileSystem df3d::base::EngineController::getInstance()->getFileSystem()
#define g_renderManager df3d::base::EngineController::getInstance()->getRenderManager()
#define g_guiManager df3d::base::EngineController::getInstance()->getGuiManager()
#define g_scriptManager df3d::base::EngineController::getInstance()->getScriptManager()
#define g_physicsWorld df3d::base::EngineController::getInstance()->getPhysicsManager()->getWorld()
#define g_audioManager df3d::base::EngineController::getInstance()->getAudioManager()