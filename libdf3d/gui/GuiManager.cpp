#include "GuiManager.h"

#include "impl/RocketInterface.h"
#include "impl/RocketKeyCodesAdapter.h"
#include <base/EngineController.h>
#include <input/InputEvents.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

#if defined(DF3D_DESKTOP)
#define ENABLE_ROCKET_DEBUGGER
#endif

namespace df3d {

GuiManager::GuiManager(int contextWidth, int contextHeight)
{
    glog << "Initializing libRocket" << logmess;

    using namespace Rocket;

    m_renderInterface.reset(new gui_impl::RenderInterface());
    m_systemInterface.reset(new gui_impl::SystemInterface());
    m_fileInterface.reset(new gui_impl::FileInterface());

    SetRenderInterface(m_renderInterface.get());
    SetSystemInterface(m_systemInterface.get());
    SetFileInterface(m_fileInterface.get());

    // Initialize core.
    if (!Core::Initialise())
        throw std::runtime_error("Can not initialize Rocket GUI library");

    // Initialize controls library.
    Controls::Initialise();

    // Create GUI context.
    m_rocketContext = Core::CreateContext("main", Core::Vector2i(contextWidth, contextHeight));

    // Initialize debugger.
#ifdef ENABLE_ROCKET_DEBUGGER
    if (!Rocket::Debugger::Initialise(m_rocketContext))
        glog << "Failed to initialize Rocket GUI debugger" << logwarn;
#endif
}

GuiManager::~GuiManager()
{
    m_rocketContext->RemoveReference();
    Rocket::Core::Shutdown();
}

RocketDocument GuiManager::loadDocument(const std::string &name)
{
    auto doc = m_rocketContext->LoadDocument(name.c_str());
    if (!doc)
        return nullptr;

    doc->RemoveReference();
    return doc;
}

void GuiManager::showDebugger(bool show)
{
    Rocket::Debugger::SetVisible(show);
}

bool GuiManager::isDebuggerVisible() const
{
    return Rocket::Debugger::IsVisible();
}

}
