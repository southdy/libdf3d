#include "df3d_pch.h"
#include "TextMeshComponent.h"

#include <render/Texture.h>
#include <render/RenderPass.h>
#include <render/GpuProgram.h>
#include <render/VertexIndexBuffer.h>
#include <render/RenderQueue.h>
#include <render/Image.h>
#include <base/Controller.h>
#include <resources/ResourceManager.h>
#include <resources/FileSystem.h>
#include <components/TransformComponent.h>
#include <scene/Node.h>
#include <SDL_ttf.h>

namespace df3d { namespace components {

shared_ptr<render::RenderPass> TextMeshComponent::createRenderPass()
{
    auto pass = make_shared<render::RenderPass>("text_mesh_render_pass");
    pass->setFrontFaceWinding(render::RenderPass::WO_CCW);
    pass->setFaceCullMode(render::RenderPass::FCM_NONE);
    pass->setPolygonDrawMode(render::RenderPass::PM_FILL);
    pass->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    pass->setBlendMode(render::RenderPass::BM_ALPHA);

    auto program = g_resourceManager->getResource<render::GpuProgram>(render::COLORED_PROGRAM_EMBED_PATH);
    pass->setGpuProgram(program);

    return pass;
}

shared_ptr<render::VertexBuffer> TextMeshComponent::createQuad(float x, float y, float w, float h)
{
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;

    float quad_pos[][2] =
    {
        { x - w2, y - h2 },
        { x + w2, y - h2 },
        { x + w2, y + h2 },
        { x + w2, y + h2 },
        { x - w2, y + h2 },
        { x - w2, y - h2 }
    };
    float quad_uv[][2] =
    {
        { 0.0, 0.0 },
        { 1.0, 0.0 },
        { 1.0, 1.0 },
        { 1.0, 1.0 },
        { 0.0, 1.0 },
        { 0.0, 0.0 }
    };

    auto vf = render::VertexFormat::create("p:3, tx:2, c:4");       // FIXME: No need in c!
    auto result = make_shared<render::VertexBuffer>(vf);

    for (int i = 0; i < 6; i++)
    {
        render::Vertex_3p2tx4c v;
        v.p.x = quad_pos[i][0];
        v.p.y = quad_pos[i][1];
        v.tx.x = quad_uv[i][0];
        v.tx.y = quad_uv[i][1];

        result->appendVertexData((const float *)&v, 1);
    }

    return result;
}

void TextMeshComponent::onDraw(render::RenderQueue *ops)
{
    if (!m_font)
        return;

    m_op.worldTransform = m_holder->transform()->getTransformation();

    if (m_op.passProps->isTransparent())
        ops->transparentOperations.push_back(m_op);
    else
        ops->notLitOpaqueOperations.push_back(m_op);
}

TextMeshComponent::TextMeshComponent(const char *fontPath, int size)
    : MeshComponent()
{
    // FIXME:
    // For now without resource manager.
    auto path = g_fileSystem->fullPath(fontPath);
    m_font = TTF_OpenFont(path.c_str(), size);
    if (!m_font)
    {
        base::glog << "Failed to initialize text renderer. Font is invalid" << TTF_GetError() << base::logwarn;
        return;
    }

    // Init render operation.
    m_op.passProps = createRenderPass();
    m_op.vertexData = createQuad(0.0f, 0.0f, 2.0f, 2.0f);
    m_op.vertexData->setUsageType(render::GB_USAGE_STATIC);
}

TextMeshComponent::~TextMeshComponent()
{
    if (m_font)
        TTF_CloseFont(m_font);
}

void TextMeshComponent::drawText(const char *text, const glm::vec4 &color)
{
    if (!m_font)
        return;

    SDL_Color colorSdl = { color.r * 255, color.g * 255, color.b * 255, 255 };
    SDL_Surface *text_surface = TTF_RenderUTF8_Blended(m_font, text, colorSdl);

    auto textureImage = make_shared<render::Image>();
    textureImage->setWithData(text_surface);
    textureImage->setInitialized();

    SDL_FreeSurface(text_surface);

    auto texture = make_shared<render::Texture>();
    texture->setImage(textureImage);

    texture->setType(render::Texture::TEXTURE_2D);
    texture->setWrapMode(render::Texture::WM_CLAMP);
    texture->setFilteringMode(render::Texture::TRILINEAR);
    texture->setMipmapped(false);

    m_op.passProps->setSampler("diffuseMap", texture);
    m_op.passProps->setDiffuseColor(1.0f, 1.0f, 1.0f, color.a);
}

glm::vec2 TextMeshComponent::getTextLength(const char *text)
{
    int w, h;
    TTF_SizeUTF8(m_font, text, &w, &h);
    
    return glm::vec2((float)w, (float)h);
}

shared_ptr<NodeComponent> TextMeshComponent::clone() const
{
    // Not implemented.
    assert(false);
    return nullptr;
}

} }