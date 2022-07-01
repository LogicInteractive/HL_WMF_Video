#define HL_NAME(n) hlmfvideo##n

#include "hl.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <wrl.h>
#include <d3d11_3.h>
#include "mfapi.h"
#include <mfmediaengine.h>
#include "MediaEnginePlayer.h"

/////////////////////////////////////////////////////////////////////////////////////

#ifndef MF_PLAYER
#define MR_PLAYER

typedef struct dx_driver_
{
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    IDXGISwapChain* swapchain;
    ID3D11RenderTargetView* renderTarget;
    D3D_FEATURE_LEVEL feature;
    int init_flags;
} dx_driver;

typedef ID3D11Resource dx_resource;
typedef ID3D11DeviceChild dx_pointer;

#endif 

ID3D11Texture2D* dxPointerToD3DTex(dx_pointer* d3d11ShaderResourceView)
{
    ID3D11ShaderResourceView* rv = (ID3D11ShaderResourceView*)d3d11ShaderResourceView;
    ID3D11Resource* rs = nullptr;
    rv->GetResource(&rs);
    ID3D11Texture2D* tex2d = reinterpret_cast<ID3D11Texture2D*>(rs);
    return tex2d;
}

HL_PRIM MediaEnginePlayer* HL_NAME(hlmf_create)(dx_driver driverStruct)
{
    ID3D11Device* d3dDevice = driverStruct.device;
    auto mplayer = new MediaEnginePlayer();
    mplayer->Initialize(d3dDevice, DXGI_FORMAT_B8G8R8A8_UNORM);
    return mplayer;
}

HL_PRIM void HL_NAME(hlmf_dispose)(MediaEnginePlayer* mplayer)
{
    delete mplayer;
    mplayer = nullptr;
}

HL_PRIM void HL_NAME(hlmf_setSource)(MediaEnginePlayer* mplayer, const char* url)
{
    if (!mplayer)
        return;

    std::wstring w;
    std::copy(url, url + strlen(url), back_inserter(w));
    const WCHAR* u = w.c_str();

    if (mplayer->isPlaying)
    {
        mplayer->SetSource(u,true);
    }
    else
    {
        mplayer->SetSource(u);
    }

    //mplayer->SetSource(L"D:/SampleVideo.mp4");
}

HL_PRIM void HL_NAME(hlmf_render)(MediaEnginePlayer* mplayer, dx_pointer* d3d11ShaderResourceView)
{
    if ((!mplayer->isInfoReady && mplayer->isPlaying))
        return;

    uint32_t videoWidth;
    uint32_t videoHeight;
    mplayer->GetNativeVideoSize(videoWidth, videoHeight);
    ID3D11Texture2D* tex2d = dxPointerToD3DTex(d3d11ShaderResourceView);

    RECT rct = { 0, 0, long(videoWidth), long(videoHeight) };
    MFVideoNormalizedRect rect = { 0.0f, 0.0f, 1.0f, 1.0f };
    mplayer->TransferFrame(tex2d, rect, rct);
}

HL_PRIM void HL_NAME(hlmf_play)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return;

    mplayer->Play();
}

HL_PRIM void HL_NAME(hlmf_stop)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return;

    mplayer->Stop();
}

HL_PRIM void HL_NAME(hlmf_pause)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return;

    mplayer->Pause();
}

HL_PRIM void HL_NAME(hlmf_resume)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return;

    mplayer->Resume();
}

HL_PRIM bool HL_NAME(hlmf_isPlaying)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return false;

    return mplayer->isPlaying;
}

HL_PRIM bool HL_NAME(hlmf_isInfoReady)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return false;

    return mplayer->isInfoReady;
}

HL_PRIM bool HL_NAME(hlmf_canPlay)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return false;

    return mplayer->canPlay;
}

HL_PRIM int HL_NAME(hlmf_getWidth)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return 0;

    uint32_t videoWidth;
    uint32_t videoHeight;
    mplayer->GetNativeVideoSize(videoWidth, videoHeight);
    return videoWidth;
}

HL_PRIM int HL_NAME(hlmf_getHeight)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return 0;

    uint32_t videoWidth;
    uint32_t videoHeight;
    mplayer->GetNativeVideoSize(videoWidth, videoHeight);
    return videoHeight;
}

HL_PRIM bool HL_NAME(hlmf_getMuted)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return false;

    return mplayer->GetMuted();
}

HL_PRIM void HL_NAME(hlmf_setMuted)(MediaEnginePlayer* mplayer,bool muted)
{
    if (!mplayer)
        return;

    mplayer->SetMuted(muted);
}

HL_PRIM double HL_NAME(hlmf_getVolume)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return 0;

    return mplayer->GetVolume();
}

HL_PRIM void HL_NAME(hlmf_setVolume)(MediaEnginePlayer* mplayer,double volume)
{
    if (!mplayer)
        return;

    mplayer->SetVolume(volume);
}

HL_PRIM double HL_NAME(hlmf_getCurrentTime)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return 0;

    return mplayer->GetCurrentTime();
}

HL_PRIM void HL_NAME(hlmf_setCurrentTime)(MediaEnginePlayer* mplayer,double seekTime)
{
    if (!mplayer)
        return;

    mplayer->SetCurrentTime(seekTime);
}

HL_PRIM double HL_NAME(hlmf_getDuration)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return 0;

    return mplayer->GetDuration();
}

HL_PRIM bool HL_NAME(hlmf_getLoop)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return false;

    return mplayer->GetLoop();
}

HL_PRIM void HL_NAME(hlmf_setLoop)(MediaEnginePlayer* mplayer,bool loop)
{
    if (!mplayer)
        return;

    mplayer->SetLoop(loop);
}

HL_PRIM bool HL_NAME(hlmf_hasAudio)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return false;

    return mplayer->HasAudio();
}

HL_PRIM bool HL_NAME(hlmf_hasVideo)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return false;

    return mplayer->HasVideo();
}

HL_PRIM bool HL_NAME(hlmf_isEnded)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return true;

    return mplayer->IsEnded();
}

HL_PRIM bool HL_NAME(hlmf_isSeeking)(MediaEnginePlayer* mplayer)
{
    if (!mplayer)
        return false;

    return mplayer->IsSeeking();
}

HL_PRIM bool HL_NAME(hlmf_canPlayType)(MediaEnginePlayer* mplayer, const char* type)
{
    if (!mplayer)
        return false;

    return mplayer->CanPlayType(type);
}

/////////////////////////////////////////////////////////////////////////////////////

#define _DRIVER _ABSTRACT(dx_driver)
#define _RESOURCE _ABSTRACT(dx_resource)
#define _POINTER _ABSTRACT(dx_pointer)
#define _MPLAYER _ABSTRACT(MediaEnginePlayer*)

DEFINE_PRIM(_MPLAYER, hlmf_create, _DRIVER);
DEFINE_PRIM(_VOID, hlmf_setSource, _MPLAYER _BYTES);
DEFINE_PRIM(_VOID, hlmf_dispose, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_play, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_stop, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_pause, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_resume, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_render, _MPLAYER _POINTER);
DEFINE_PRIM(_BOOL, hlmf_isPlaying, _MPLAYER);
DEFINE_PRIM(_BOOL, hlmf_isInfoReady, _MPLAYER);
DEFINE_PRIM(_BOOL, hlmf_canPlay, _MPLAYER);
DEFINE_PRIM(_I32,  hlmf_getWidth, _MPLAYER);
DEFINE_PRIM(_I32,  hlmf_getHeight, _MPLAYER);
DEFINE_PRIM(_BOOL, hlmf_getMuted, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_setMuted, _MPLAYER _BOOL);
DEFINE_PRIM(_F64,  hlmf_getVolume, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_setVolume, _MPLAYER _F64);
DEFINE_PRIM(_F64,  hlmf_getCurrentTime, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_setCurrentTime, _MPLAYER _F64);
DEFINE_PRIM(_F64,  hlmf_getDuration, _MPLAYER);
DEFINE_PRIM(_BOOL, hlmf_getLoop, _MPLAYER);
DEFINE_PRIM(_VOID, hlmf_setLoop, _MPLAYER _BOOL);
DEFINE_PRIM(_BOOL, hlmf_hasAudio, _MPLAYER);
DEFINE_PRIM(_BOOL, hlmf_hasVideo, _MPLAYER);
DEFINE_PRIM(_BOOL, hlmf_isEnded, _MPLAYER);
DEFINE_PRIM(_BOOL, hlmf_isSeeking, _MPLAYER);
DEFINE_PRIM(_BOOL, hlmf_canPlayType, _MPLAYER _BYTES);
