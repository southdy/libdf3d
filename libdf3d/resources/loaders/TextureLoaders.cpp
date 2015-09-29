#include "TextureLoaders.h"

#include <base/Service.h>
#include <resources/FileDataSource.h>
#include <utils/JsonHelpers.h>

#define STB_IMAGE_IMPLEMENTATION
#ifndef STB_DO_ERROR_PRINT
#define STBI_NO_FAILURE_STRINGS     // not thread-safe
#endif
#include <stb/stb_image.h>

namespace df3d { namespace resources {

// stb image loader helpers.
namespace
{
    // fill 'data' with 'size' bytes.  return number of bytes actually read
    int read(void *user, char *data, int size)
    {
        auto dataSource = static_cast<FileDataSource*>(user);
        return dataSource->getRaw(data, size);
    }

    // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
    void skip(void *user, int n)
    {
        if (n < 0)
        {
            // TODO:
            assert(false);
        }

        auto dataSource = static_cast<FileDataSource*>(user);
        dataSource->seek(n, std::ios_base::cur);
    }

    // returns nonzero if we are at end of file/data
    int eof(void *user)
    {
        auto dataSource = static_cast<FileDataSource*>(user);
        return dataSource->tell() >= dataSource->getSize();
    }

    unique_ptr<render::PixelBuffer> loadPixelBuffer(shared_ptr<FileDataSource> source)
    {
        if (!source)
        {
            base::glog << "Failed to load pixel buffer from file source. Source is null." << base::logwarn;
            return nullptr;
        }

        stbi_io_callbacks callbacks;
        callbacks.read = read;
        callbacks.skip = skip;
        callbacks.eof = eof;
        auto dataSource = source.get();

        int x, y, bpp;
        auto pixels = stbi_load_from_callbacks(&callbacks, dataSource, &x, &y, &bpp, 0);
        if (!pixels)
        {
            base::glog << "Can not load image" << source->getPath() << base::logwarn;
#ifdef STB_DO_ERROR_PRINT
            base::glog << stbi_failure_reason() << base::logwarn;
#endif
            return nullptr;
        }

        auto fmt = PixelFormat::INVALID;

        if (bpp == STBI_rgb)
            fmt = PixelFormat::RGB;
        else if (bpp == STBI_rgb_alpha)
            fmt = PixelFormat::RGBA;
        else if (bpp == STBI_grey)
            fmt = PixelFormat::GRAYSCALE;
        else
            base::glog << "Parsed image with an invalid bpp" << base::logwarn;

        auto result = make_unique<render::PixelBuffer>(x, y, pixels, fmt);

        stbi_image_free(pixels);

        return result;
    }
}

Texture2DManualLoader::Texture2DManualLoader(unique_ptr<render::PixelBuffer> pixelBuffer, render::TextureCreationParams params)
    : m_pixelBuffer(std::move(pixelBuffer)), m_params(params)
{

}

render::Texture2D* Texture2DManualLoader::load()
{
    return new render::Texture2D(*m_pixelBuffer, m_params);
}

Texture2DFSLoader::Texture2DFSLoader(const std::string &path, const render::TextureCreationParams &params, ResourceLoadingMode lm)
    : FSResourceLoader(lm),
    m_pathToTexture(path),
    m_params(params)
{

}

render::Texture2D* Texture2DFSLoader::createDummy()
{
    return new render::Texture2D(m_params);
}

void Texture2DFSLoader::decode(shared_ptr<FileDataSource> source)
{
    m_pixelBuffer = loadPixelBuffer(source);
}

void Texture2DFSLoader::onDecoded(Resource *resource)
{
    if (!m_pixelBuffer)
        return;

    auto texture = static_cast<render::Texture2D*>(resource);
    texture->createGLTexture(*m_pixelBuffer);

    /*
    base::glog << "Cleaning up" << m_pixelBuffer->getSizeInBytes() / 1024.0f << "KB of CPU copy of pixel data" << base::logdebug;
    */

    // Explicitly remove CPU copy of pixel buffer in order to prevent caching.
    // Instead, will load new copy from FS when rebinding occurs.
    m_pixelBuffer.reset();
}

TextureCubeFSLoader::TextureCubeFSLoader(const std::string &path, const render::TextureCreationParams &params, ResourceLoadingMode lm)
    : FSResourceLoader(lm),
    m_params(params),
    m_jsonPath(path)
{

}

render::TextureCube* TextureCubeFSLoader::createDummy()
{
    return new render::TextureCube(m_params);
}

void TextureCubeFSLoader::decode(shared_ptr<FileDataSource> source)
{
    std::string buffer(source->getSize(), 0);
    source->getRaw(&buffer[0], source->getSize());

    auto jsonRoot = utils::jsonLoadFromSource(buffer);
    if (jsonRoot.empty())
        return;

    auto srcPathDir = gsvc().filesystem.getFileDirectory(source->getPath());

    auto getSource = [&srcPathDir](const std::string &texturePath)
    {
        auto fullPath = gsvc().filesystem.pathConcatenate(srcPathDir, texturePath);
        return gsvc().filesystem.openFile(fullPath);
    };

    m_pixelBuffers[render::CUBE_FACE_POSITIVE_X] = loadPixelBuffer(getSource(jsonRoot["positive_x"].asString()));
    m_pixelBuffers[render::CUBE_FACE_NEGATIVE_X] = loadPixelBuffer(getSource(jsonRoot["negative_x"].asString()));
    m_pixelBuffers[render::CUBE_FACE_POSITIVE_Y] = loadPixelBuffer(getSource(jsonRoot["positive_y"].asString()));
    m_pixelBuffers[render::CUBE_FACE_NEGATIVE_Y] = loadPixelBuffer(getSource(jsonRoot["negative_y"].asString()));
    m_pixelBuffers[render::CUBE_FACE_POSITIVE_Z] = loadPixelBuffer(getSource(jsonRoot["positive_z"].asString()));
    m_pixelBuffers[render::CUBE_FACE_NEGATIVE_Z] = loadPixelBuffer(getSource(jsonRoot["negative_z"].asString()));
}

void TextureCubeFSLoader::onDecoded(Resource *resource)
{
    for (const auto &pb : m_pixelBuffers)
    {
        if (!pb)
        {
            base::glog << "Failed to decode cube texture. Image(s) invalid." << base::logwarn;
            return;
        }
    }

    auto texture = static_cast<render::TextureCube*>(resource);

    texture->createGLTexture(m_pixelBuffers);

    // Clean up main memory.
    for (int i = 0; i < render::CUBE_FACES_COUNT; i++)
        m_pixelBuffers[i].reset();
}

} }