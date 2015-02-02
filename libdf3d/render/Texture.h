#pragma once

#include "OpenGLCommon.h"
#include "RenderingCapabilities.h"

namespace df3d { namespace render {

class Texture
{
public:
    enum class WrapMode
    {
        WRAP,
        CLAMP
    };

protected:
    WrapMode m_wrapMode = WrapMode::CLAMP;
    GLuint m_glid = 0;

    boost::optional<TextureFiltering> m_filtering;
    boost::optional<bool> m_mipmapped;
    boost::optional<int> m_anisotropyLevel;

    // Helpers.
    static bool isPot(size_t v);
    static size_t getNextPot(size_t v);
    static GLint getGlFilteringMode(TextureFiltering filtering, bool mipmapped);
    static GLint getGlWrapMode(WrapMode mode);

    static void setupGlTextureFiltering(GLenum glType, TextureFiltering filtering, bool mipmapped);
    static void setupGlWrapMode(GLenum glType, WrapMode wrapMode);

public:
    Texture();
    virtual ~Texture();

    WrapMode wrapMode() const { return m_wrapMode; }
    unsigned getGLId() const { return m_glid; }

    TextureFiltering filtering() const;
    bool isMipmapped() const;
    int anisotropyLevel() const;

    void setFilteringMode(TextureFiltering newFiltering);
    void setMipmapped(bool hasMipmaps);
    void setWrapMode(WrapMode mode);
    void setMaxAnisotropy(int aniso);

    virtual bool bind(size_t unit) = 0;
    virtual void unbind() = 0;
};

} }
