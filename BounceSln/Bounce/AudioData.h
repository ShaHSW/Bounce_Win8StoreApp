#pragma once

#include "BasicReaderWriter.h"

ref class AudioData
{
internal:
    AudioData(void);

    void LoadAudio(
        _In_ Platform::String^ filename,
        _In_ IXAudio2* xaudio
        );

    void Play(
        _In_ float volume
        );
    //concurrency::task<Platform::Array<byte>^> LoadAudioAsync(
    //    _In_ Platform::String^ filename,
    //    _Inout_ LPWAVEFORMATEX pWaveFormat
    //    );
private:
    BasicReaderWriter^ m_basicReaderWriter;
    WAVEFORMATEX                     m_wfx;
    bool                             m_audioAvailable;
    IXAudio2SourceVoice*             m_sourceVoice;
    Platform::Array<byte>^           m_soundData;
};

