#include "pch.h"
#include "AudioData.h"

using namespace Microsoft::WRL;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace std;
using namespace concurrency;

AudioData::AudioData(void):
    m_audioAvailable(false)
{
    m_basicReaderWriter = ref new BasicReaderWriter();
}


void AudioData::LoadAudio(
    _In_ Platform::String^ filename,
    _In_ IXAudio2* xaudio
    )
{
    DX::ThrowIfFailed(
        MFStartup(MF_VERSION)
        );
    
    ComPtr<IMFSourceReader> spSourceReader;
    DX::ThrowIfFailed(
        MFCreateSourceReaderFromURL(
            Platform::String::Concat(m_basicReaderWriter->GetFolder(), filename)->Data(),
            nullptr,
            &spSourceReader
            )
        );

    // Set the decoded output format as PCM.
    // XAudio2 on Windows can process PCM and ADPCM-encoded buffers.
    // When using MediaFoundation, this sample always decodes into PCM.
    ComPtr<IMFMediaType> spMediaType;
    DX::ThrowIfFailed(
        MFCreateMediaType(&spMediaType)
        );

    DX::ThrowIfFailed(
        spMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio)
        );

    DX::ThrowIfFailed(
        spMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM)
        );

    DX::ThrowIfFailed(
        spSourceReader->SetCurrentMediaType(static_cast<uint32>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, spMediaType.Get())
        );

    // Get the complete WAVEFORMAT from the Media Type.
    Microsoft::WRL::ComPtr<IMFMediaType> spOutputMediaType;
    DX::ThrowIfFailed(
        spSourceReader->GetCurrentMediaType(static_cast<uint32>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &spOutputMediaType)
        );

    UINT32 size = 0;
    WAVEFORMATEX* waveFormat;
    DX::ThrowIfFailed(
        MFCreateWaveFormatExFromMFMediaType(spOutputMediaType.Get(), &waveFormat, &size)
        );

    CopyMemory(&m_wfx, waveFormat, sizeof(WAVEFORMATEX));
    CoTaskMemFree(waveFormat);

    PROPVARIANT propVariant;
    DX::ThrowIfFailed(
        spSourceReader->GetPresentationAttribute(static_cast<uint32>(MF_SOURCE_READER_MEDIASOURCE), MF_PD_DURATION, &propVariant)
        );

    // 'duration' is in 100ns units; convert to seconds, and round up
    // to the nearest whole byte.
    LONGLONG duration = propVariant.uhVal.QuadPart;
    unsigned int maxStreamLengthInBytes =
        static_cast<unsigned int>(
        ((duration * static_cast<ULONGLONG>(m_wfx.nAvgBytesPerSec)) + 10000000) /
            10000000
            );

    m_soundData = ref new Platform::Array<byte>(maxStreamLengthInBytes);

    ComPtr<IMFSample> spSample;
    ComPtr<IMFMediaBuffer> spMediaBuffer;
    DWORD flags = 0;

    int positionInData = 0;
    bool done = false;
    while (!done)
    {
        DX::ThrowIfFailed(
            spSourceReader->ReadSample(static_cast<uint32>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &flags, nullptr, &spSample)
            );

        if (spSample != nullptr)
        {
            DX::ThrowIfFailed(
                spSample->ConvertToContiguousBuffer(&spMediaBuffer)
                );

            BYTE *audioData = nullptr;
            DWORD sampleBufferLength = 0;
            DX::ThrowIfFailed(
                spMediaBuffer->Lock(&audioData, nullptr, &sampleBufferLength)
                );

            for (DWORD i = 0; i < sampleBufferLength; i++)
            {
                m_soundData[positionInData++] = audioData[i];
            }
        }
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            done = true;
        }
    }

    // Create a source voice for this sound effect.
    DX::ThrowIfFailed(
        xaudio->CreateSourceVoice(
            &m_sourceVoice,
            &m_wfx
            )
        );
    m_audioAvailable = true;
}

void AudioData::Play(_In_ float volume)
{
    XAUDIO2_BUFFER buffer = {0};

    if (!m_audioAvailable)
    {
        // Audio is not available so just return.
        return;
    }

    // Interrupt sound effect if it is currently playing.
    DX::ThrowIfFailed(
        m_sourceVoice->Stop()
        );
    DX::ThrowIfFailed(
        m_sourceVoice->FlushSourceBuffers()
        );

    // Queue the memory buffer for playback and start the voice.
    buffer.AudioBytes = m_soundData->Length;
    buffer.pAudioData = m_soundData->Data;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    DX::ThrowIfFailed(
        m_sourceVoice->SetVolume(volume)
        );
    DX::ThrowIfFailed(
        m_sourceVoice->SubmitSourceBuffer(&buffer)
        );
    DX::ThrowIfFailed(
        m_sourceVoice->Start()
        );
}