#pragma warning(push)
#pragma warning(disable : 4467)
#include <wrl.h>
#pragma warning(pop)

#include <d3d11_3.h>
#include <d3d10.h>

#if defined(NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_5.h>
#endif

#include <DirectXMath.h>
#include <mfmediaengine.h>
#include "MediaEnginePlayer.h"

using Microsoft::WRL::ComPtr;

namespace
{
    class MediaEngineNotify : public IMFMediaEngineNotify
    {
        long m_cRef;
        IMFNotify* m_pCB;

    public:
        MediaEngineNotify() noexcept :
            m_cRef(1),
            m_pCB(nullptr)
        {
        }

        STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
        {
            if (__uuidof(IMFMediaEngineNotify) == riid)
            {
                *ppv = static_cast<IMFMediaEngineNotify*>(this);
            }
            else
            {
                *ppv = nullptr;
                return E_NOINTERFACE;
            }

            AddRef();

            return S_OK;
        }

        STDMETHODIMP_(ULONG) AddRef()
        {
            return InterlockedIncrement(&m_cRef);
        }

        STDMETHODIMP_(ULONG) Release()
        {
            LONG cRef = InterlockedDecrement(&m_cRef);
            if (cRef == 0)
            {
                delete this;
            }
            return cRef;
        }

        void SetCallback(IMFNotify* pCB)
        {
            m_pCB = pCB;
        }

        // EventNotify is called when the Media Engine sends an event.
        STDMETHODIMP EventNotify(DWORD meEvent, DWORD_PTR param1, DWORD)
        {
            if (meEvent == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE)
            {
                SetEvent(reinterpret_cast<HANDLE>(param1));
            }
            else
            {
                m_pCB->OnMediaEngineEvent(meEvent);
            }

            return S_OK;
        }
    };
}

MediaEnginePlayer::MediaEnginePlayer()
{

}

MediaEnginePlayer::~MediaEnginePlayer()
{
    Shutdown();
    MFShutdown();
}

void MediaEnginePlayer::Initialize(ID3D11Device* device, DXGI_FORMAT format)
{
    // Initialize Media Foundation
    HRESULT hr = MFStartup(MF_VERSION);

    // Create our own device to avoid threading issues
    ComPtr<IDXGIDevice> dxgiDevice;
    hr = device->QueryInterface(IID_PPV_ARGS(dxgiDevice.GetAddressOf()));

    ComPtr<IDXGIAdapter> adapter;
    hr = dxgiDevice->GetAdapter(&adapter);

    D3D_FEATURE_LEVEL fLevel = device->GetFeatureLevel();

    hr = D3D11CreateDevice(
        adapter.Get(),
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        &fLevel,
        1,
        D3D11_SDK_VERSION,
        d3ddevice.GetAddressOf(),
        nullptr,
        nullptr
    );

    ComPtr<ID3D10Multithread> multithreaded;
    hr = d3ddevice.As(&multithreaded);
    multithreaded->SetMultithreadProtected(TRUE);

    // Setup Media Engine
    Microsoft::WRL::ComPtr<IMFDXGIDeviceManager> dxgiManager;
    UINT resetToken;
    hr = MFCreateDXGIDeviceManager(&resetToken, dxgiManager.GetAddressOf());
    hr = dxgiManager->ResetDevice(d3ddevice.Get(), resetToken);

    // Create our event callback object.
    ComPtr<MediaEngineNotify> spNotify = new MediaEngineNotify();
    if (spNotify == nullptr)
        hr = E_OUTOFMEMORY;

    spNotify->SetCallback(this);

    // Set configuration attribiutes.
    ComPtr<IMFAttributes> attributes;
    hr = MFCreateAttributes(attributes.GetAddressOf(), 1);
    hr = attributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, reinterpret_cast<IUnknown*>(dxgiManager.Get()));
    hr = attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, reinterpret_cast<IUnknown*>(spNotify.Get()));
    hr = attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, format);

    // Create MediaEngine.
    ComPtr<IMFMediaEngineClassFactory> mfFactory;
    hr = CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_ALL, IID_PPV_ARGS(mfFactory.GetAddressOf()));
    hr = mfFactory->CreateInstance(0, attributes.Get(), mediaEngine.ReleaseAndGetAddressOf());
    // Create MediaEngineEx
    hr = mediaEngine.As(&engineEx);
}

void MediaEnginePlayer::Shutdown()
{
    if (mediaEngine)
    {
        mediaEngine->Shutdown();
    }
    isPlaying = false;
}

void MediaEnginePlayer::Play()
{
    if (isPlaying)
        return;

    if (mediaEngine)
    {
        mediaEngine->Play();
        isPlaying = true;
        isFinished = false;
    }
}

void MediaEnginePlayer::Stop()
{
    Shutdown();
}

void MediaEnginePlayer::Pause()
{
    HRESULT hr = mediaEngine->Pause();
}

void MediaEnginePlayer::Resume()
{
    if (mediaEngine)
    {
        mediaEngine->Play();
    }
}

bool MediaEnginePlayer::GetMuted()
{
    if (mediaEngine)
        return mediaEngine->GetMuted();
    else
        return false;
}

void MediaEnginePlayer::SetMuted(bool muted)
{
    if (mediaEngine)
    {
        HRESULT hr = mediaEngine->SetMuted(muted);
    }
}

double MediaEnginePlayer::GetVolume()
{
    if (mediaEngine)
        return mediaEngine->GetVolume();
    else
        return 0;
}

void MediaEnginePlayer::SetVolume(double volume)
{
    if (mediaEngine)
    {
        HRESULT hr = mediaEngine->SetVolume(volume);
    }
}

double MediaEnginePlayer::GetCurrentTime()
{
    if (mediaEngine)
        return mediaEngine->GetCurrentTime();
    else
        return 0;
}

void MediaEnginePlayer::SetCurrentTime(double seekTime)
{
    if (mediaEngine)
        HRESULT hr = mediaEngine->SetCurrentTime(seekTime);
}

bool MediaEnginePlayer::GetLoop()
{
    if (mediaEngine)
        return mediaEngine->GetLoop();
    else
        return 0;
}

void MediaEnginePlayer::SetLoop(bool loop)
{
    if (mediaEngine)
        HRESULT hr = mediaEngine->SetLoop(loop);
}

double MediaEnginePlayer::GetDuration()
{
    if (mediaEngine)
        return mediaEngine->GetDuration();
    else
        return 0;
}

double MediaEnginePlayer::GetPlaybackRate()
{
    if (mediaEngine)
        return mediaEngine->GetPlaybackRate();
    else
        return 0;
}

bool MediaEnginePlayer::HasAudio()
{
    if (mediaEngine)
        return mediaEngine->HasAudio();
    else
        return false;
}

bool MediaEnginePlayer::HasVideo()
{
    if (mediaEngine)
        return mediaEngine->HasVideo();
    else
        return false;
}

bool MediaEnginePlayer::IsEnded()
{
    if (mediaEngine)
        return mediaEngine->IsEnded();
    else
        return false;
}

bool MediaEnginePlayer::IsSeeking()
{
    if (mediaEngine)
        return mediaEngine->IsSeeking();
    else
        return false;
}

bool MediaEnginePlayer::CanPlayType(const char* type)
{
    if (!mediaEngine)
        return false;

    int wslen = MultiByteToWideChar(CP_ACP, 0, type, strlen(type), 0, 0);
    BSTR bstr = SysAllocStringLen(0, wslen);
    MultiByteToWideChar(CP_ACP, 0, type, strlen(type), bstr, wslen);
    if (bstr == 0)
    {
        SysFreeString(bstr);
        return false;
    }

    MF_MEDIA_ENGINE_CANPLAY ans;
    mediaEngine->CanPlayType(bstr,&ans);
    SysFreeString(bstr);
    bstr = NULL;

    if (ans == MF_MEDIA_ENGINE_CANPLAY_NOT_SUPPORTED)
        return false;
    else if (ans == MF_MEDIA_ENGINE_CANPLAY_PROBABLY || ans == MF_MEDIA_ENGINE_CANPLAY_MAYBE)
        return true;
    else
        return false;
}

//m_mediaEngine->SetAutoPlay(true);
//m_mediaEngine->SetPlaybackRate(1);

void MediaEnginePlayer::SetSource(_In_z_ const wchar_t* sourceUri, bool autoplay)
{
    static BSTR bstrURL = nullptr;

    if (bstrURL != nullptr)
    {
        ::CoTaskMemFree(bstrURL);
        bstrURL = nullptr;
    }

    size_t cchAllocationSize = 1 + ::wcslen(sourceUri);
    bstrURL = reinterpret_cast<BSTR>(::CoTaskMemAlloc(sizeof(wchar_t)*(cchAllocationSize)));

    if (bstrURL == 0)
    {
        HRESULT hr = E_OUTOFMEMORY;
    }

    wcscpy_s(bstrURL, cchAllocationSize, sourceUri);

    if (!isPlaying)
    {
        isPlaying = false;
        isInfoReady = false;
        isFinished = false;
    }
    else
    {
        isInfoReady = false;
    }

    if (mediaEngine)
    {
        mediaEngine->SetAutoPlay(autoplay);
        mediaEngine->SetSource(bstrURL);
    }
}

bool MediaEnginePlayer::TransferFrame(ID3D11Texture2D* texture, MFVideoNormalizedRect rect, RECT rcTarget)
{
    if (mediaEngine != nullptr && isPlaying)
    {
        LONGLONG pts;
        if (mediaEngine->OnVideoStreamTick(&pts) == S_OK)
        {
            // The texture must have been created with D3D11_RESOURCE_MISC_SHARED
            ComPtr<IDXGIResource> dxgiTexture;
            texture->QueryInterface(IID_PPV_ARGS(dxgiTexture.GetAddressOf()));
            HANDLE textureHandle;
            dxgiTexture->GetSharedHandle(&textureHandle);

            ComPtr<ID3D11Texture2D> mediaTexture;
            if (SUCCEEDED(d3ddevice->OpenSharedResource(textureHandle, IID_PPV_ARGS(mediaTexture.GetAddressOf()))))
            {
                if (mediaEngine->TransferVideoFrame(mediaTexture.Get(), &rect, &rcTarget, &bgColor) == S_OK)
                    return true;
            }
            else
            {
                printf("failed share ");
            }
        }
    }

    return false;
}

void MediaEnginePlayer::OnMediaEngineEvent(uint32_t meEvent)
{
    switch (meEvent)
    {
        case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
            isInfoReady = true;
            break;

        case MF_MEDIA_ENGINE_EVENT_CANPLAY:
            canPlay = true;
            break;

        case MF_MEDIA_ENGINE_EVENT_PLAY:
            break;

        case MF_MEDIA_ENGINE_EVENT_PAUSE:
            break;

        case MF_MEDIA_ENGINE_EVENT_ENDED:
            isFinished = true;
            break;

        case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
            break;

        case MF_MEDIA_ENGINE_EVENT_ERROR:
            if (mediaEngine)
            {
                ComPtr<IMFMediaError> error;
                if (SUCCEEDED(mediaEngine->GetError(&error)))
                {
                    USHORT errorCode = error->GetErrorCode();
                    HRESULT hr = error->GetExtendedErrorCode();
                    printf("ERROR: Media Foundation Event Error %u (%08X)\n", errorCode, static_cast<unsigned int>(hr));
                }
                else
                {
                    printf("ERROR: Media Foundation Event Error *FAILED GetError*\n");
                }
            }
            break;
    }
}

void MediaEnginePlayer::GetNativeVideoSize(uint32_t& cx, uint32_t& cy)
{
    cx = cy = 0;
    if (mediaEngine && isInfoReady)
    {
        DWORD x, y;
        HRESULT hr = mediaEngine->GetNativeVideoSize(&x, &y);

        cx = x;
        cy = y;
    }
}
