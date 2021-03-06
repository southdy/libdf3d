#include "ScriptManager.h"

#include <sqstdaux.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdblob.h>
#include <cstdarg>
#include <cstdio>
#include <sqrat/sqrat.h>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/resources/ResourceManager.h>
#include <df3d/engine/resources/ResourceFileSystem.h>
#include <df3d/engine/resources/ResourceDataSource.h>

namespace df3d {

static void printfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
#ifndef DF3D_DISABLE_LOGGING
    va_list arglist;
    va_start(arglist, s);

    glog.vprint(Log::CHANNEL_GAME, s, arglist);

    va_end(arglist);
#endif
}

void ScriptManager::initialize()
{
    DFLOG_MESS("Starting Squirrel");

    m_squirrel = sq_open(1024);

    sq_pushroottable(m_squirrel); // push the root table where the std function will be registered

    if (!SQ_SUCCEEDED(sqstd_register_mathlib(m_squirrel)))
        throw std::runtime_error("Failed to register squirrel math library");

    if (!SQ_SUCCEEDED(sqstd_register_stringlib(m_squirrel)))
        throw std::runtime_error("Failed to register squirrel string library");

    if (!SQ_SUCCEEDED(sqstd_register_bloblib(m_squirrel)))
        throw std::runtime_error("Failed to register squirrel blob library");

    sq_pop(m_squirrel, 1); // pops the root table

    sqstd_seterrorhandlers(m_squirrel);
    sq_setprintfunc(m_squirrel, printfunc, printfunc);

    // Bind df3d code.
    // FIXME: figure out how to share (static member functions in sqrat)
    //script_impl::bindDf3d(m_squirrel);
}

void ScriptManager::shutdown()
{
    sq_close(m_squirrel);
    m_squirrel = nullptr;
}

bool ScriptManager::doFile(const char *fileName)
{
    if (auto file = svc().resourceManager().getFS().open(fileName))
    {
        std::string buffer(file->getSize(), 0);
        file->read(&buffer[0], buffer.size());

        DFLOG_DEBUG("Executing %s", fileName);

        svc().resourceManager().getFS().close(file);

        return doString(buffer.c_str());
    }
    else
    {
        DFLOG_WARN("Failed to execute %s. File doesn't exist", fileName);
        return false;
    }
}

bool ScriptManager::doString(const char *str)
{
    Sqrat::Script squirrelScript(m_squirrel);

    Sqrat::string errMsg;
    if (!squirrelScript.CompileString(str, errMsg))
    {
        DFLOG_WARN("Failed to compile squirrel script: %s", errMsg.c_str());
        DEBUG_BREAK();

        return false;
    }

#if !defined (SCRAT_NO_ERROR_CHECKING)
    if (!squirrelScript.Run(errMsg))
    {
        DFLOG_WARN("Failed to run squirrel script: %s", errMsg.c_str());
        DEBUG_BREAK();

        return false;
    }
#else 
    squirrelScript.Run();
#endif

    return true;
}

void ScriptManager::gc()
{
    if (!SQ_SUCCEEDED(sq_collectgarbage(m_squirrel)))
        DFLOG_WARN("Squirrel: Failed to collect garbage");
}

}
