#include "../src/DirectSoundStream.hpp"

extern "C" HRESULT __stdcall DirectSoundStreamProbe(
    CSoundManagerStream *mgr, CStreamingSound **outStream,
    unsigned char *data, DWORD dataSize, BgmTrackInfoStream *info,
    GUID guid, DWORD notifySize, HANDLE hEvent,
    CWaveFileStream *w, CSoundStream *s, CStreamingSound *st,
    IDirectSoundBufferStreamView *buf, const char *path, DWORD *outRead)
{
    HRESULT hr = mgr->CreateStreamingFromMemory(outStream, data, dataSize, info, guid, notifySize, hEvent);
    hr |= w->OpenFile(path, info);
    hr |= w->ReopenTrack(info, dataSize);
    hr |= w->ResetFile(true, dataSize);
    hr |= w->ResetFile(false, 0);
    hr |= w->Read(data, dataSize, outRead);
    hr |= w->Read(data + 4, dataSize + 4, NULL);
    delete w;

    CSoundStream *sNew = new CSoundStream(&buf, dataSize, w);
    hr |= s->RecreateBuffers();
    hr |= s->FillBufferWithSound(buf, TRUE);
    hr |= s->FillBufferWithSound(buf, FALSE);
    hr |= s->Play();
    hr |= s->Stop(1);
    hr |= s->Stop(0);
    hr |= s->Resume();
    hr |= s->ReopenTrack(info, dataSize);
    delete sNew;

    CStreamingSound *stNew = new CStreamingSound(buf, dataSize, w, notifySize);
    hr |= st->HandleWaveStreamNotification(TRUE);
    hr |= st->ResetStream();
    delete stNew;
    return hr;
}
