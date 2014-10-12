#include "df3d_pch.h"
#include "GuiManager.h"

#include <base/Controller.h>
#include <base/InputEvents.h>
#include <scripting/ScriptManager.h>
#include "RocketInterface.h"
#include "RocketKeyBindings.h"
#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

#if defined(__WINDOWS__)
#define ENABLE_ROCKET_DEBUGGER
#endif

namespace df3d { namespace gui {

GuiManager::GuiManager()
    : m_rocketContext(nullptr)
{
}

GuiManager::~GuiManager()
{
}

bool GuiManager::init(int contextWidth, int contextHeight)
{
    base::glog << "Initializing Rocket GUI" << base::logmess;

    using namespace Rocket;

    m_renderInterface.reset(new GuiRenderInterface());
    m_systemInterface.reset(new GuiSystemInterface());
    m_fileInterface.reset(new GuiFileInterface());

    SetRenderInterface(m_renderInterface.get());
    SetSystemInterface(m_systemInterface.get());
    SetFileInterface(m_fileInterface.get());

    // Initialize core.
    if (!Core::Initialise())
    {
    	base::glog << "Can not initialize Rocket GUI library" << base::logmess;
        return false;
    }
    
    // Initialize controls library.
    Controls::Initialise();

#if defined(DF3D_USES_PYTHON)
    // Import RocketPython first.
    if (!g_scriptManager->doString("import pyrocketcore"))
    {
        base::glog << "Failed to init Python GUI subsystem" << base::logcritical;
        return false;
    }
#endif

    // Create GUI context.
    m_rocketContext = Core::CreateContext("main", Core::Vector2i(contextWidth, contextHeight));

    // Initialize debugger.
#ifdef ENABLE_ROCKET_DEBUGGER
    if (!Rocket::Debugger::Initialise(m_rocketContext))
        base::glog << "Failed to initialize Rocket GUI debugger" << base::logwarn;
#endif

    return m_rocketContext != nullptr;
}

void GuiManager::shutdown()
{
    m_rocketContext->RemoveReference();
    Rocket::Core::Shutdown();
}

void GuiManager::update(float dt)
{
    if (m_rocketContext)
        m_rocketContext->Update();
}

void GuiManager::render()
{
    if (m_rocketContext)
        m_rocketContext->Render();
}

void GuiManager::processMouseButtonEvent(const SDL_MouseButtonEvent &ev)
{
    int buttonIdx;
    if (ev.button == SDL_BUTTON_LEFT)
        buttonIdx = 0;
    else if (ev.button == SDL_BUTTON_RIGHT)
        buttonIdx = 1;
    else
        return;

    if (ev.type == SDL_MOUSEBUTTONDOWN)
        m_rocketContext->ProcessMouseButtonDown(buttonIdx, 0);
    else if (ev.type == SDL_MOUSEBUTTONUP)
        m_rocketContext->ProcessMouseButtonUp(buttonIdx, 0);
}

void GuiManager::processMouseMotionEvent(const base::MouseMotionEvent &ev)
{
    m_rocketContext->ProcessMouseMove(ev.x, ev.y, 0);
}

void GuiManager::processMouseWheelEvent(const SDL_MouseWheelEvent &ev)
{
    m_rocketContext->ProcessMouseWheel(-ev.y, 0);
}

void GuiManager::processKeyDownEvent(const SDL_KeyboardEvent &ev)
{
    auto k = RocketKeycodeFromSdl(ev.keysym.sym);
    if (k != Rocket::Core::Input::KI_UNKNOWN)
        m_rocketContext->ProcessKeyDown(k, RocketKeyModifierFromSdl(ev.keysym));
}

void GuiManager::processKeyUpEvent(const SDL_KeyboardEvent &ev)
{
    auto k = RocketKeycodeFromSdl(ev.keysym.sym);
    if (k != Rocket::Core::Input::KI_UNKNOWN)
        m_rocketContext->ProcessKeyUp(k, RocketKeyModifierFromSdl(ev.keysym));
}

void GuiManager::processTextInputEvent(const SDL_TextInputEvent &ev)
{
    m_rocketContext->ProcessTextInput(ev.text);
}

RocketDocument GuiManager::loadDocument(const char *name)
{
    auto doc = m_rocketContext->LoadDocument(name);
    if (!doc)
        return nullptr;

    doc->RemoveReference();
    return doc;
}

RocketDocument GuiManager::loadDocumentFromMemory(const std::string &data)
{
    auto doc = m_rocketContext->LoadDocumentFromMemory(data.c_str());
    if (!doc)
        return nullptr;

    doc->RemoveReference();
    return doc;
}

void GuiManager::loadFont(const char *path)
{
    Rocket::Core::FontDatabase::LoadFontFace(path);
}

void GuiManager::setDebuggerVisible(bool visible)
{
#ifdef ENABLE_ROCKET_DEBUGGER
    Rocket::Debugger::SetVisible(visible);
#endif
}

bool GuiManager::isDebuggerVisible()
{
#ifdef ENABLE_ROCKET_DEBUGGER
    return Rocket::Debugger::IsVisible();
#else
    return false;
#endif
}

Rocket::Core::Context *GuiManager::getRocketContext()
{
    return m_rocketContext;
}

} }
