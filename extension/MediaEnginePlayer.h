#pragma once

#include "hl.h"
#include "mfapi.h"
#include <mfmediaengine.h>

//-------------------------------------------------------------------------------------
class IMFNotify
{
public:
    virtual ~IMFNotify() = default;

    IMFNotify(const IMFNotify&) = delete;
    IMFNotify& operator=(const IMFNotify&) = delete;

    IMFNotify(IMFNotify&&) = delete;
    IMFNotify& operator=(IMFNotify&&) = delete;

    virtual void OnMediaEngineEvent(uint32_t meEvent) = 0;

protected:
    IMFNotify() = default;
};


class MediaEnginePlayer : public IMFNotify
{
public:
    MediaEnginePlayer();
    ~MediaEnginePlayer();

    void Initialize(ID3D11Device* device, DXGI_FORMAT format);
    void Shutdown();

    void Play();
    void Stop();
    void Pause();
    void Resume();
    
    bool GetMuted();
    void SetMuted(bool muted);

    double GetVolume();
    void SetVolume(double volume);

    double GetCurrentTime();
    void SetCurrentTime(double seekTime);

    bool GetLoop();
    void SetLoop(bool loop);

    double GetDuration();
    double GetPlaybackRate();

    bool HasAudio();
    bool HasVideo();
    bool IsEnded();
    bool IsSeeking(); 
    bool CanPlayType(const char* type);

    void SetSource(_In_z_ const wchar_t* sourceUri, bool autoplay = false);
    bool TransferFrame(ID3D11Texture2D* texture, MFVideoNormalizedRect rect, RECT rcTarget);

    // Callbacks
    void OnMediaEngineEvent(uint32_t meEvent);
    void GetNativeVideoSize(uint32_t& cx, uint32_t& cy);

    bool isPlaying = false;
    bool isInfoReady = false;
    bool isFinished = false;
    bool canPlay = false;

private:
    Microsoft::WRL::ComPtr<ID3D11Device>        d3ddevice;
    Microsoft::WRL::ComPtr<IMFMediaEngine>      mediaEngine;
    Microsoft::WRL::ComPtr<IMFMediaEngineEx>    engineEx;

    MFARGB bgColor = { 0,0,0,0 };
};
