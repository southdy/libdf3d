#include "GpuProgramUniform.h"

#include "OpenGLCommon.h"

namespace df3d {

SharedUniformType getSharedTypeForUniform(const std::string &name)
{
    if (name == "u_worldViewProjectionMatrix")
        return SharedUniformType::WORLD_VIEW_PROJECTION_MATRIX_UNIFORM;
    else if (name == "u_worldViewMatrix")
        return SharedUniformType::WORLD_VIEW_MATRIX_UNIFORM;
    else if (name == "u_worldViewMatrix3x3")
        return SharedUniformType::WORLD_VIEW_3X3_MATRIX_UNIFORM;
    else if (name == "u_viewMatrixInverse")
        return SharedUniformType::VIEW_INVERSE_MATRIX_UNIFORM;
    else if (name == "u_viewMatrix")
        return SharedUniformType::VIEW_MATRIX_UNIFORM;
    else if (name == "u_projectionMatrix")
        return SharedUniformType::PROJECTION_MATRIX_UNIFORM;
    else if (name == "u_worldMatrix")
        return SharedUniformType::WORLD_MATRIX_UNIFORM;
    else if (name == "u_worldMatrixInverse")
        return SharedUniformType::WORLD_INVERSE_MATRIX_UNIFORM;
    else if (name == "u_normalMatrix")
        return SharedUniformType::NORMAL_MATRIX_UNIFORM;
    else if (name == "u_globalAmbient")
        return SharedUniformType::GLOBAL_AMBIENT_UNIFORM;
    else if (name == "u_cameraPosition")
        return SharedUniformType::CAMERA_POSITION_UNIFORM;
    else if (name == "u_fogDensity")
        return SharedUniformType::FOG_DENSITY_UNIFORM;
    else if (name == "u_fogColor")
        return SharedUniformType::FOG_COLOR_UNIFORM;
    else if (name == "u_pixelSize")
        return SharedUniformType::PIXEL_SIZE_UNIFORM;
    else if (name == "u_elapsedTime")
        return SharedUniformType::ELAPSED_TIME_UNIFORM;
    else if (name == "material.ambient")
        return SharedUniformType::MATERIAL_AMBIENT_UNIFORM;
    else if (name == "material.diffuse")
        return SharedUniformType::MATERIAL_DIFFUSE_UNIFORM;
    else if (name == "material.specular")
        return SharedUniformType::MATERIAL_SPECULAR_UNIFORM;
    else if (name == "material.emissive")
        return SharedUniformType::MATERIAL_EMISSIVE_UNIFORM;
    else if (name == "material.shininess")
        return SharedUniformType::MATERIAL_SHININESS_UNIFORM;
    else if (name == "current_light.diffuse")
        return SharedUniformType::SCENE_LIGHT_DIFFUSE_UNIFORM;
    else if (name == "current_light.specular")
        return SharedUniformType::SCENE_LIGHT_SPECULAR_UNIFORM;
    else if (name == "current_light.position")
        return SharedUniformType::SCENE_LIGHT_POSITION_UNIFORM;
    else if (name == "current_light.constantAttenuation")
        return SharedUniformType::SCENE_LIGHT_KC_UNIFORM;
    else if (name == "current_light.linearAttenuation")
        return SharedUniformType::SCENE_LIGHT_KL_UNIFORM;
    else if (name == "current_light.quadraticAttenuation")
        return SharedUniformType::SCENE_LIGHT_KQ_UNIFORM;

    return SharedUniformType::COUNT;
}

GpuProgramUniform::GpuProgramUniform(const std::string &name)
    : m_name(name)
{
    assert(!m_name.empty());

    // Try to set it shared.
    m_sharedId = getSharedTypeForUniform(m_name);
}

GpuProgramUniform::~GpuProgramUniform()
{

}

void GpuProgramUniform::update(const void *data) const
{
    switch (m_glType)
    {
    case GL_SAMPLER_2D:
        glUniform1iv(m_location, 1, (GLint *)data);
        break;
    case GL_SAMPLER_CUBE:
        glUniform1iv(m_location, 1, (GLint *)data);
        break;
    case GL_INT:
        glUniform1iv(m_location, 1, (GLint *)data);
        break;
    case GL_FLOAT:
        glUniform1fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC2:
        glUniform2fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC3:
        glUniform3fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_VEC4:
        glUniform4fv(m_location, 1, (GLfloat *)data);
        break;
    case GL_FLOAT_MAT3:
        glUniformMatrix3fv(m_location, 1, GL_FALSE, (GLfloat *)data);
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(m_location, 1, GL_FALSE, (GLfloat *)data);
        break;
    default:
        glog << "Failed to update GpuProgramUniform. Unknown uniform type" << logwarn;
        break;
    }
}

}
