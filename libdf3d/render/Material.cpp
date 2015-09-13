#include "df3d_pch.h"
#include "Material.h"

#include "Technique.h"

namespace df3d { namespace render {

shared_ptr<Technique> Material::findTechnique(const std::string &name)
{
    auto findFn = [&name](const shared_ptr<Technique> &tech)
    {
        return tech->getName() == name;
    };

    auto found = std::find_if(m_techniques.cbegin(), m_techniques.cend(), findFn);
    if (found == m_techniques.cend())
        return nullptr;
    
    return *found;
}

Material::Material(const std::string &name)
    : m_name(name)
{
    if (name.empty())
        base::glog << "Creating material with empty name" << base::logwarn;
}

Material::Material(const Material &other)
    : m_name(other.m_name)
{
    for (const auto &tech : other.m_techniques)
        m_techniques.push_back(make_shared<Technique>(*tech));

    if (other.m_currentTechnique)
        m_currentTechnique = findTechnique(other.m_currentTechnique->getName());
}

Material& Material::operator= (Material other)
{
    std::swap(m_name, other.m_name);
    std::swap(m_techniques, other.m_techniques);
    std::swap(m_currentTechnique, other.m_currentTechnique);

    return *this;
}

Material::~Material()
{

}

const std::string &Material::getName() const
{
    return m_name;
}

void Material::appendTechnique(const Technique &technique)
{
    if (findTechnique(technique.getName()))
    {
        base::glog << "Trying to add duplicate technique" << technique.getName() << "to material" << m_name << base::logwarn;
        return;
    }

    m_techniques.push_back(make_shared<Technique>(technique));
}

void Material::setCurrentTechnique(const std::string &name)
{
    if (m_currentTechnique && m_currentTechnique->getName() == name)
        return;

    auto found = findTechnique(name);
    if (found)
        m_currentTechnique = found;
    else
        base::glog << "Trying to set empty technique" << name << "to material" << m_name << base::logwarn;
}

shared_ptr<Technique> Material::getCurrentTechnique()
{
    return m_currentTechnique;
}

shared_ptr<Technique> Material::getTechnique(const std::string &name)
{
    return findTechnique(name);
}

size_t Material::getTechniquesCount() const
{
    return m_techniques.size();
}

} }
