#include "df3d_pch.h"
#include "ScriptManager.h"

#include <lua/lua.hpp>
#include <selene/selene.h>

#include <base/SystemsMacro.h>
#include "lua/df3d_bindings.h"

namespace df3d { namespace scripting {

ScriptManager::ScriptManager()
{
    base::glog << "Initializing Lua" << base::logmess;

    m_state = make_unique<sel::State>(true);

    bindGlm(m_state.get());
}

ScriptManager::~ScriptManager()
{

}

bool ScriptManager::doFile(const char *fileName)
{
    auto fullp = g_fileSystem->fullPath(fileName);

    base::glog << "Executing" << fullp << base::logdebug;

    if (!m_state->Load(fullp))
    {
        base::glog << "Lua script execution failed." << base::logwarn;
        return false;
    }

    return true;
}

bool ScriptManager::doString(const char *str)
{
    return false;
}

void ScriptManager::printError()
{

}

} }
