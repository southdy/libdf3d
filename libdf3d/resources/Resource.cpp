#include "df3d_pch.h"
#include "Resource.h"

#include <base/Service.h>

namespace df3d { namespace resources {

Resource::Resource()
{
    // guid = g_resmgr->getGuid()??
    static unsigned long resourcesCount = 0;

    std::ostringstream ss;
    ss << "unnamed_resource_" << resourcesCount++;
    m_guid = ss.str();
}

void Resource::setGUID(const ResourceGUID &guid)
{ 
    if (!IsGUIDValid(guid))
    {
        base::glog << "Trying to set invalid guid" << guid << base::logwarn;
        return;
    }

    m_guid = guid; 
}

const ResourceGUID &Resource::getGUID() const
{ 
    return m_guid; 
}

const std::string &Resource::getFilePath() const
{
    return m_guid;
}

bool IsGUIDValid(const ResourceGUID &guid)
{
    return !guid.empty();
}

ResourceGUID CreateGUIDFromPath(const std::string &path)
{
    return gsvc().filesystem.fullPath(path);
}

} }
