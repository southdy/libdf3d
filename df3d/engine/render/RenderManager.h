#pragma once

#include "RenderCommon.h"
#include "Viewport.h"
#include "RenderPass.h"
#include <df3d/engine/EngineInitParams.h>

namespace df3d {

class IRenderBackend;
class Viewport;
class Material;
class RenderQueue;
class World;
class RenderOperation;
class GpuProgramSharedState;

class DF3D_DLL RenderManager : NonCopyable
{
    friend class EngineController;

    EngineInitParams m_initParams;
    unique_ptr<RenderQueue> m_renderQueue;
    unique_ptr<IRenderBackend> m_renderBackend;
    unique_ptr<GpuProgramSharedState> m_sharedState;

    Viewport m_viewport;

    // Forward rendering stuff.
    unique_ptr<RenderPass> m_ambientPassProps;
    PassParamHandle m_ambientMtlParam;
    bool m_blendModeOverriden = false;
    bool m_depthTestOverriden = false;
    bool m_depthWriteOverriden = false;

    shared_ptr<Texture> m_whiteTexture;
    shared_ptr<GpuProgram> m_simpleLightingProgram, m_coloredProgram, m_ambientPassProgram;

    void onFrameBegin();
    void onFrameEnd();
    void doRenderWorld(World &world);

    void bindPass(RenderPass *pass);

public:
    RenderManager(EngineInitParams params);
    ~RenderManager();

    void initialize();
    void shutdown();

    void loadEmbedResources();
    void forgetEmbedResources();

    void drawWorld(World &world);
    void drawRenderOperation(const RenderOperation &op);

    const Viewport& getViewport() const;
    const RenderingCapabilities& getRenderingCapabilities() const;
    FrameStats getFrameStats() const;

    IRenderBackend& getBackend();
};

}
