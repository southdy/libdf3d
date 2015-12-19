#include "AudioManager.h"

#include "impl/OpenALCommon.h"
#include <base/EngineController.h>
#include <scene/Camera.h>

namespace df3d {

struct AudioManager::Impl
{
    ALCdevice *m_device = nullptr;
    ALCcontext *m_context = nullptr;
};

AudioManager::AudioManager()
    : m_pimpl(new AudioManager::Impl())
{
    glog << "Initializing OpenAL" << logmess;

    std::string devices;
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") != AL_FALSE)
        devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
    else
        devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

    std::string defaultDevice;
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") != AL_FALSE)
        defaultDevice = alcGetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
    else
        defaultDevice = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    glog << "Available playback devices:" << devices << logmess;
    glog << "Default playback device:" << defaultDevice << logmess;

    m_pimpl->m_device = alcOpenDevice(nullptr);
    if (!m_pimpl->m_device)
        throw std::runtime_error("Can not open audio device");

    m_pimpl->m_context = alcCreateContext(m_pimpl->m_device, nullptr);
    if (!m_pimpl->m_context)
        throw std::runtime_error("Can not create OpenAL context");

    alcMakeContextCurrent(m_pimpl->m_context);

    printOpenALError();
}

AudioManager::~AudioManager()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_pimpl->m_context);
    alcCloseDevice(m_pimpl->m_device);
}

}
