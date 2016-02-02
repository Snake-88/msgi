#include "stdafx.h"
#include "Sound.hpp"
#include "File.hpp"

#include <io.h> // TODO: Use the games I/O funcs or still will probably explode

using QWORD = __int64;

static IDirectSoundBuffer* gSndBuffer_dword_77E2D0 = nullptr;
static IDirectSoundBuffer* g128_Sound_buffers_dword_77DCA0[256] = {};
static IDirectSoundBuffer* gSndBuffer_dword_77E0A0 = nullptr;
static DWORD gSndState_dword_77E2D4 = 0;
static DWORD gSoundFxIdx_dword_77D884 = 0;
static DWORD* dword_68D058; // part of below array?
static DWORD* dword_68D05C; // 21 array?
static DWORD* dword_68D084; // part of below array?
static DWORD* dword_68D088; // 10 array?
static IDirectSoundBuffer* g64_dword_77D774[64] = {};
static DWORD dword_77E2DC = 0;
static DWORD dword_77E2F0 = 0;
static DWORD gWaveFile_dword_68CE30 = 0;
static DWORD dword_77E2D8;
static IDirectSound* gDSound_dword_77E2C0 = nullptr;
static IDirectSoundBuffer* gSoundBuffer_dword_77E1B0 = nullptr;
static DWORD* gFxState_dword_77D8A0 = 0; // 256 array?
static DWORD dword_77E1B4 = 0;
static DWORD dword_77E1C4 = 0;
static DWORD dword_77D87C = 0;
static DWORD gBlockAlign_dword_77E1DC = 0;
static LONG gSndVolume_dword_77D88C = 0;
static QWORD qword_77E1A8 = 0;
static DWORD dword_77D874 = 0;
static QWORD qword_77D898 = 0;
static IDirectSoundBuffer* gSndSamp1_dword_77E2C4 = nullptr;
static IDirectSoundBuffer* gSndSamp2_dword_77E2C8 = nullptr;
static DWORD gSamp1PlayPos_dword_77E1D0 = 0;
static DWORD dword_68E318 = 0;
static DWORD dword_77E2F8 = 0;
static DWORD dword_68CE18 = 0;
DWORD dword_77D894 = 0;
DWORD dword_77E2F4 = 0;

// Many unknowns in these
char** off_68D0B4 = 0;
char* byte_68D0B1 = 0;
char* byte_68D0B0 = 0;
__int16 unk_68D630[1550] = {};
DWORD dword_68CE34 = 0;

unsigned char* byte_68CE38 = nullptr;
unsigned char* byte_68CE39 = nullptr;
DWORD dword_77E2E0 = 0;
DWORD dword_68CE2C = 0;
DWORD dword_77E2EC = 0;
DWORD dword_77E2E8 = 0;
DWORD dword_77E1C0 = 0;

VAR(DWORD, dword_77E2CC, 0x77E2CC);

// 0x0052269C
signed int __cdecl Sound_Init(HWND hwnd)
{
    signed int result = 0;

    if (true) // TODO: Link fix
        //  if (DirectSoundCreate(0, &gDSound_dword_77E2C0, 0))
    {
        result = 0;
    }
    else if (gDSound_dword_77E2C0->SetCooperativeLevel(hwnd, 3))
    {
        result = 0;
    }
    else if (Sound_CreatePrimarySoundBuffer())
    {
        Sound_InitFx();
        result = Sound_CreateSecondarySoundBuffer();
    }
    else
    {
        result = 0;
    }
    return result;
}

// 0x005227AD
char __cdecl Sound_CharUpperChangeQ(char value)
{
    char ret;

    if (value < '0' || value > '9')
    {
        if (toupper(value) < 'A' || toupper(value) > 'F')
            ret = 0;
        else
            ret = toupper(value) - '7';
    }
    else
    {
        ret = value - '0';
    }
    return ret;
}

// 0x00522BCE
signed int __cdecl Sound_CleanUpRelated()
{
    for (int i = 0; i < 64; ++i)
    {
        if (g64_dword_77D774[i])
        {
            g64_dword_77D774[i]->Stop();
            g64_dword_77D774[i]->Release();
            g64_dword_77D774[i] = 0;
        }
    }

    for (int i = 128; i < 256; ++i)
    {
        if (g128_Sound_buffers_dword_77DCA0[i])
        {
            g128_Sound_buffers_dword_77DCA0[i]->Stop();
            g128_Sound_buffers_dword_77DCA0[i]->Release();
            g128_Sound_buffers_dword_77DCA0[i] = 0;
        }
    }
    dword_77E2DC = 0;
    Sound_CloseWavStopQ();
    gSoundFxIdx_dword_77D884 = -1;
    return 1;
}

// 0x00522466
int __cdecl Sound_CloseWavStopQ()
{
    int result = 0;

    if (!dword_77E2DC)
    {
        dword_77E2F0 = 0;
    }

    if (gSndBuffer_dword_77E2D0)
    {
        result = gSndBuffer_dword_77E2D0->Stop();
    }

    if (gWaveFile_dword_68CE30 != -1)
    {
        // TODO: msg_close
        //result = _close(gWaveFile_dword_68CE30);
    }

    gWaveFile_dword_68CE30 = -1;
    gSndState_dword_77E2D4 = 0;
    dword_77E2D8 = 0;
    return result;
}

// 0x00523A44
signed int __cdecl Sound_CreateBufferQ(int numChannels, signed int bitsPerSample, int samplesPerSecond, int a4, int a5)
{
    DSBUFFERDESC bufferDesc;
    WAVEFORMATEX waveFormat;

    int blockAlign = bitsPerSample / 8 * numChannels;
    dword_77E1B4 = (a5 + 4) * blockAlign * a4;

    if (gDSound_dword_77E2C0)
    {
        waveFormat.wFormatTag = 1;
        waveFormat.nChannels = numChannels;
        waveFormat.nSamplesPerSec = samplesPerSecond;
        waveFormat.nAvgBytesPerSec = blockAlign * samplesPerSecond;
        waveFormat.nBlockAlign = blockAlign;
        waveFormat.wBitsPerSample = bitsPerSample;
        waveFormat.cbSize = 0;
        memset(&bufferDesc, 0, 36u);
        bufferDesc.dwSize = 36;
        bufferDesc.dwFlags = 0x100C8;
        bufferDesc.dwBufferBytes = dword_77E1B4;
        bufferDesc.lpwfxFormat = &waveFormat;
        gDSound_dword_77E2C0->CreateSoundBuffer(&bufferDesc, &gSndBuffer_dword_77E0A0, 0);
    }

    dword_77E1C4 = a5;
    dword_77D87C = a4;
    gBlockAlign_dword_77E1DC = blockAlign;

    if (gSndBuffer_dword_77E0A0)
    {
        gSndBuffer_dword_77E0A0->SetCurrentPosition(0);
    }
    return 1;
}

// 0x00522601
bool __cdecl Sound_CreatePrimarySoundBuffer()
{
    bool result;
    DSBUFFERDESC bufferDesc;
    WAVEFORMATEX waveFormat;

    waveFormat.wFormatTag = 1;
    waveFormat.nChannels = 2;
    waveFormat.nSamplesPerSec = 22050;
    waveFormat.nAvgBytesPerSec = 88200;
    waveFormat.nBlockAlign = 4;
    waveFormat.wBitsPerSample = 16;
    waveFormat.cbSize = 0;

    memset(&bufferDesc, 0, 36u);
    bufferDesc.dwSize = 36;
    bufferDesc.dwFlags = 1;
    bufferDesc.lpwfxFormat = 0;
    bufferDesc.dwBufferBytes = 0;

    if (gDSound_dword_77E2C0->CreateSoundBuffer(&bufferDesc, &gSoundBuffer_dword_77E1B0, 0))
    {
        result = 0;
    }
    else
    {
        result = gSoundBuffer_dword_77E1B0->SetFormat(&waveFormat) == 0;
    }
    return result;
}

// 0x00521982
signed int __cdecl Sound_CreateSecondarySoundBuffer()
{
    signed int result;
    DSBUFFERDESC bufferDesc;
    WAVEFORMATEX waveFormat;

    waveFormat.wFormatTag = 1;
    waveFormat.nChannels = 2;
    waveFormat.nSamplesPerSec = 22050;
    waveFormat.nAvgBytesPerSec = 44100;
    waveFormat.nBlockAlign = 2;
    waveFormat.wBitsPerSample = 8;
    waveFormat.cbSize = 0;
    memset(&bufferDesc, 0, 36u);
    bufferDesc.dwSize = 36;
    bufferDesc.dwFlags = 0x10088;
    bufferDesc.dwBufferBytes = 176400;
    bufferDesc.lpwfxFormat = &waveFormat;
    if (gDSound_dword_77E2C0->CreateSoundBuffer(&bufferDesc, &gSndBuffer_dword_77E2D0, 0))
    {
        result = 0;
    }
    else
    {
        gSndBuffer_dword_77E2D0->SetCurrentPosition(0);
        result = 1;
    }
    return result;
}

// 0x0052236D
__int64 __cdecl Sound_FadeQ(int a1)
{
    __int64 result;
    LARGE_INTEGER Frequency;
    __int64 freq;
    LARGE_INTEGER PerformanceCount;
    __int64 v5;

    if (gSndBuffer_dword_77E2D0)
    {
        if (gSndState_dword_77E2D4)
        {
            gSndBuffer_dword_77E2D0->GetVolume(&gSndVolume_dword_77D88C);
            QueryPerformanceFrequency(&Frequency);
            freq = Frequency.QuadPart;
            QueryPerformanceCounter(&PerformanceCount);
            qword_77E1A8 = PerformanceCount.QuadPart;
            dword_77D874 = -10000;
            if (a1)
            {
                switch (a1)
                {
                case 1:
                    v5 = 750i64;
                    break;
                case 2:
                    v5 = 2000i64;
                    break;
                case 3:
                    v5 = 3000i64;
                    break;
                }
            }
            else
            {
                v5 = 250i64;
            }
            result = freq * v5 / 1000;
            qword_77D898 = freq * v5 / 1000;
            dword_77E2D8 = 1;
        }
    }
    return result;
}

// 0x005234EA
bool __cdecl Sound_GetSamp1PosQ()
{
    bool result; // eax@2

    if (dword_68E318 == -1)
    {
        if (dword_77E2CC)
        {
            result = 0;
        }
        else if (gSndSamp1_dword_77E2C4)
        {
            gSndSamp1_dword_77E2C4->GetCurrentPosition(&gSamp1PlayPos_dword_77E1D0, 0);
            if (gSamp1PlayPos_dword_77E1D0 <= dword_77E2F8)
            {
                gSamp1PlayPos_dword_77E1D0 += 176400;
            }
            result = dword_77E2F8 + 28672 <= gSamp1PlayPos_dword_77E1D0;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        result = 1;
    }
    return result;
}

// 0x005224BE
int __cdecl Sound_GetSomeStateQ()
{
    return gSndState_dword_77E2D4;
}

// 0x00522A33
int __cdecl Sound_InitFx()
{
    int result;
    char fxFileName[256] = {};
    for (int fxNum = 0; fxNum < 128; ++fxNum)
    {
        sprintf(fxFileName, "%s0x%02x.wav", "efx/", fxNum);
        Sound_LoadBufferFromFile(fxFileName);
        gFxState_dword_77D8A0[fxNum] = 0;
        result = fxNum + 1;
    }
    return result;
}

// 0x005227FF
signed int __cdecl Sound_LoadBufferFromFile(const char *fileName)
{
    signed int result;
    size_t v3;
    char v4;
    DWORD sizeToRead;
    DSBUFFERDESC bufferDesc;
    void* v9;
    DWORD v10;
    size_t fileNameLength; 
    WAVEFORMATEX waveFormat;
    void *soundBuffer;

    gSoundFxIdx_dword_77D884 = -1;
    fileNameLength = strlen(fileName) - 6;
    const char v1 = 16 * Sound_CharUpperChangeQ(fileName[fileNameLength]);
    const unsigned __int8 idx = Sound_CharUpperChangeQ(fileName[fileNameLength + 1]) + v1;
    memset(&bufferDesc, 0, 36u);
    bufferDesc.dwSize = 36;
    bufferDesc.dwFlags = 0x100C8;
    bufferDesc.lpwfxFormat = &waveFormat;
    FILE* File = File_LoadDirFileQ(fileName, 0);
    if (File
        && (File_NormalRead(File, &v4, 20u), File_NormalRead(File, &waveFormat, 18u) == 18)
        && (File_NormalRead(File, &v4, 2u), File_NormalRead(File, &sizeToRead, 4u) == 4))
    {
        bufferDesc.dwBufferBytes = sizeToRead;
        if (gDSound_dword_77E2C0)
        {
            if (gDSound_dword_77E2C0->CreateSoundBuffer(
                &bufferDesc,
                (IDirectSoundBuffer **)g128_Sound_buffers_dword_77DCA0[idx],
                0))
            {
                return 0;
            }
            g128_Sound_buffers_dword_77DCA0[idx]->SetCurrentPosition(0);
            if (g128_Sound_buffers_dword_77DCA0[idx]->Lock(
                0,
                sizeToRead,
                &soundBuffer,
                &sizeToRead,
                &v9,
                &v10,
                0) == 0x88780096)
            {
                g128_Sound_buffers_dword_77DCA0[idx]->Restore();
                g128_Sound_buffers_dword_77DCA0[idx]->Lock(
                    0,
                    sizeToRead,
                    &soundBuffer,
                    &sizeToRead,
                    &v9,
                    &v10,
                    0);
            }
            v3 = File_NormalRead(File, soundBuffer, sizeToRead);
            if (v3 != sizeToRead)
            {
                return 0;
            }
            g128_Sound_buffers_dword_77DCA0[idx]->Unlock(
                soundBuffer,
                sizeToRead,
                v9,
                v10);
        }
        File_CloseQ(File);
        result = 1;
    }
    else
    {
        File_CloseQ(File);
        result = 0;
    }
    return result;
}

void __cdecl Sound_LoadFxRelatedQ(const char *Str1)
{
    char soundFileName[256];
    unsigned __int16 soundNum;
    int sampleSet;
    int v5;

    v5 = 0;
    sampleSet = 0xFF;
    for (int i = 0; i < 176; ++i)
    {
        if (!strcmp(Str1, off_68D0B4[2 * i]))
        {
            v5 = byte_68D0B1[8 * i];
            sampleSet = byte_68D0B0[8 * i];
            break;
        }
    }
    dword_77E2F4 = v5;
    for (int i = 0; i < 1550; ++i) // 82 is biggest samples in a set, 1550 is total number of sample set samples, excluding main/ones in root dir
    {
        soundNum = unk_68D630[i];
        if ((((signed int)soundNum >> 8) & 127) == sampleSet)
        {
            sprintf(soundFileName, "%ssample%02x/0x%02x.wav", "efx/", sampleSet, soundNum);
            Sound_LoadBufferFromFile(soundFileName);
            gFxState_dword_77D8A0[soundNum] = 0;
        }
    }
    dword_68CE34 = sampleSet;
}

// 0x00522B8D
void __cdecl Sound_LoadFxRelatedQ2(const char *Str1)
{
    for (int i = 0; i < 64; ++i)
    {
        g64_dword_77D774[i] = 0;
    }
    dword_77D894 = 0;
    Sound_LoadFxRelatedQ(Str1);
}

// 0x00521A54
signed int __cdecl Sound_MxdWavRelated(signed int a1)
{
    int v2;
    bool v3;
    bool v4;
    LARGE_INTEGER Frequency;
    char soundFileName;
    __int64 freq;
    unsigned __int8 sndNumber;
    void* v9;
    void* v10;
    DWORD v11;
    DWORD nNumberOfBytesToRead;
    LARGE_INTEGER PerformanceCount;

    if ((a1 & 0xFFFF00) == 0xFFFF00)
    {
        if (a1 >= 6 && a1 <= 13 || a1 == 255)
        {
            if (a1 == 255)
            {
                Sound_CloseWavStopQ();
            }
            else
            {
                Sound_FadeQ((a1 - 6) % 4);
                v4 = a1 >= 6 && a1 <= 9;
                dword_77E2DC = v4;
            }
            if (!gSndBuffer_dword_77E2D0)
                Sound_CloseWavStopQ();
            return 1;
        }
        if (a1 < 3 || a1 > 5)
            return 1;
        a1 = dword_77E2F0;
    }
    if (a1 > 8 || a1 < 1)
        return 1;
    if (a1 == dword_77E2F0)
    {
        if (!dword_77E2DC)
            return 1;
    }
    else
    {
        dword_77E2DC = 0;
        Sound_CloseWavStopQ();
    }
    dword_77E2DC = 0;
    dword_77E2F0 = a1;
    if (dword_77E2F4 == 59 || dword_77E2F4 >= 62)
    {
        switch (a1)
        {
        case 1:
            sndNumber = byte_68CE38[2 * dword_77E2F4];
            break;
        case 2:
            sndNumber = byte_68CE39[2 * dword_77E2F4];
            break;
        case 3:
            sndNumber = 33;
            break;
        case 4:
            sndNumber = 32;
            break;
        case 5:
            sndNumber = 30;
            break;
        case 6:
            sndNumber = 31;
            break;
        case 7:
            sndNumber = 27;
            break;
        default:
            sndNumber = byte_68CE38[2 * dword_77E2F4];
            break;
        }
        goto LABEL_74;
    }
    if (a1 == 1)
    {
        sndNumber = byte_68CE38[2 * dword_77E2F4];
        goto LABEL_74;
    }
    if (a1 == 2)
    {
        sndNumber = 6;
        goto LABEL_74;
    }
    if (a1 != 3)
    {
        if (a1 != 4)
            return 1;
        if (dword_77E2F4 != 29 && dword_77E2F4 != 45)
            sndNumber = byte_68CE39[2 * dword_77E2F4];
        else
            sndNumber = byte_68CE38[2 * dword_77E2F4];
        goto LABEL_74;
    }
    if (dword_77E2F4 > 36)
    {
        if (dword_77E2F4 != 39 && dword_77E2F4 != 42 && (dword_77E2F4 <= 43 || dword_77E2F4 > 46))
            return 1;
    }
    else if (dword_77E2F4 < 35 && dword_77E2F4 != 18)
    {
        if (dword_77E2F4 == 19)
        {
            sndNumber = 13;
            goto LABEL_74;
        }
        if (dword_77E2F4 <= 23 || dword_77E2F4 > 25 && (dword_77E2F4 <= 26 || dword_77E2F4 > 32))
            return 1;
    }
    sndNumber = byte_68CE39[2 * dword_77E2F4];
LABEL_74:
    v3 = sndNumber == 20 || sndNumber == 32 || sndNumber == 33;
    dword_77E2E0 = v3;
    dword_68CE2C = -1;
    if (!sndNumber)
        return 1;
    sprintf(&soundFileName, "%s0x%02x.wav", "mdx/", sndNumber);
    gWaveFile_dword_68CE30 = _open(&soundFileName, 0x8000);
    if (gWaveFile_dword_68CE30 == -1)
        return 0;
    _lseek(gWaveFile_dword_68CE30, 40, 0);
    if (_read(gWaveFile_dword_68CE30, &dword_77E2EC, 4u) != 4)
        return 0;
    if (gSndBuffer_dword_77E2D0)
    {
        if (gSndBuffer_dword_77E2D0->Lock(
            0,
            88200,
            &v10,
            &nNumberOfBytesToRead,
            &v9,
            &v11,
            0) == 0x88780096)
        {
            gSndBuffer_dword_77E2D0->Restore();
            gSndBuffer_dword_77E2D0->Lock(0, 88200, &v10, &nNumberOfBytesToRead, &v9, &v11, 0);
        }
        v2 = _read(gWaveFile_dword_68CE30, v10, nNumberOfBytesToRead);
        if (v2 != nNumberOfBytesToRead)
        {
            _close(gWaveFile_dword_68CE30);
            gSndBuffer_dword_77E2D0->Unlock(v10, nNumberOfBytesToRead, v9, v11);
            return 0;
        }
        gSndBuffer_dword_77E2D0->Unlock(v10, nNumberOfBytesToRead, v9, v11);
    }
    dword_77E2E8 = 88200;
    QueryPerformanceFrequency(&Frequency);
    freq = Frequency.QuadPart;
    QueryPerformanceCounter(&PerformanceCount);
    qword_77E1A8 = PerformanceCount.QuadPart;
    dword_77D874 = dword_77E1C0;
    if (dword_77E2E0)
    {
        gSndVolume_dword_77D88C = dword_77E1C0;
        qword_77D898 = 0i64;
    }
    else
    {
        gSndVolume_dword_77D88C = -10000;
        qword_77D898 = 500 * freq / 1000;
    }
    if (gSndBuffer_dword_77E2D0)
    {
        gSndBuffer_dword_77E2D0->SetCurrentPosition( 0);
        gSndBuffer_dword_77E2D0->SetVolume(gSndVolume_dword_77D88C);
        gSndBuffer_dword_77E2D0->Play(0, 0, 1);
    }
    dword_77E2D8 = 0;
    gSndState_dword_77E2D4 = 1;
    dword_77E2EC -= 88200;
    return 1;
}

// 0x005231A9
int __cdecl Sound_PlaySample()
{
    int result;

    if (gSoundFxIdx_dword_77D884 != -1)
    {
        result = gSoundFxIdx_dword_77D884;
        if (g128_Sound_buffers_dword_77DCA0[gSoundFxIdx_dword_77D884])
        {
            result = g128_Sound_buffers_dword_77DCA0[gSoundFxIdx_dword_77D884]->Play(
                0,
                0,
                DSBPLAY_LOOPING);
        }
    }

    if (gSndBuffer_dword_77E2D0)
    {
        if (gSndState_dword_77E2D4 == 1)
        {
            result = gSndBuffer_dword_77E2D0->Play(0, 0, DSBPLAY_LOOPING);
        }
    }
    
    if (gSndBuffer_dword_77E0A0)
    {
        result = gSndBuffer_dword_77E0A0->Play(0, 0, DSBPLAY_LOOPING);
    }

    return result;
}

// 0x0052307F
void __cdecl Sound_PlaySampleRelated(IDirectSoundBuffer* pSoundBuffer, int a2, int a3, signed int a4)
{
    int pan;
    int vol;

    if (pSoundBuffer)
    {
        if (a2 < 0)
        {
            pan = Sound_TableUnknown1(100 * (a2 + 32) / 32, -10000, 0);
        }
        else
        {
            pan = -Sound_TableUnknown1(100 * (32 - a2) / 32, -10000, 0);
        }
        if (a4 > -2500)
        {
            vol = Sound_TableUnknown1(100 * a3 / 63, -2500, a4);
        }
        else
        {
            vol = a4;
        }
        pSoundBuffer->SetPan(pan);
        pSoundBuffer->SetVolume(vol);
    }
}

// 0x00523A1F
void __cdecl Sound_ReleaseBufferQ()
{
    if (gSndBuffer_dword_77E0A0)
    {
        gSndBuffer_dword_77E0A0->Release();
        gSndBuffer_dword_77E0A0 = 0;
    }
}

// 0x00521A18
void __cdecl Sound_ReleaseSecondaryBuffer()
{
    dword_77E2F0 = 0;
    if (gSndBuffer_dword_77E2D0)
    {
        gSndBuffer_dword_77E2D0->Stop();
        gSndBuffer_dword_77E2D0->Release();
        gSndBuffer_dword_77E2D0 = 0;
    }
}

// 0x005226EB
void __cdecl Sound_ShutDown()
{
    Sound_ReleaseSecondaryBuffer();

    for (int i = 0; i < 128; ++i)
    {
        if (g128_Sound_buffers_dword_77DCA0[i])
        {
            g128_Sound_buffers_dword_77DCA0[i]->Stop();
            g128_Sound_buffers_dword_77DCA0[i]->Release();
            g128_Sound_buffers_dword_77DCA0[i] = 0;
        }
    }

    if (gSoundBuffer_dword_77E1B0)
    {
        gSoundBuffer_dword_77E1B0->Stop();
        gSoundBuffer_dword_77E1B0->Release();
        gSoundBuffer_dword_77E1B0 = 0;
    }

    if (gDSound_dword_77E2C0)
    {
        gDSound_dword_77E2C0->Release();
        gDSound_dword_77E2C0 = 0;
    }
}

// 0x00523466
signed int __cdecl Sound_Stop2Samples()
{
    //sprintf(byte_77E0A4, "Play Stopped\n");
    //OutputDebugStringA(byte_77E0A4);

    if (gSndSamp1_dword_77E2C4)
    {
        gSndSamp1_dword_77E2C4->Stop();
        gSndSamp1_dword_77E2C4->Release();
        gSndSamp1_dword_77E2C4 = 0;
    }

    if (gSndSamp2_dword_77E2C8)
    {
        gSndSamp2_dword_77E2C8->Stop();
        gSndSamp2_dword_77E2C8->Release();
        gSndSamp2_dword_77E2C8 = 0;
    }

    return 1;
}

// 0x0052313B
int __cdecl Sound_StopSample()
{
    int result;

    if (gSoundFxIdx_dword_77D884 != -1)
    {
        result = gSoundFxIdx_dword_77D884;
        if (g128_Sound_buffers_dword_77DCA0[gSoundFxIdx_dword_77D884])
        {
            result = g128_Sound_buffers_dword_77DCA0[gSoundFxIdx_dword_77D884]->Stop();
        }
    }

    if (gSndBuffer_dword_77E2D0)
    {
        result = gSndBuffer_dword_77E2D0->Stop();
    }

    if (gSndBuffer_dword_77E0A0)
    {
        result = gSndBuffer_dword_77E0A0->Stop();
    }

    return result;
}

// 0x00521898
int __cdecl Sound_TableUnknown1(int a1, int rate, int vol)
{
    int v4;
    int v5;
    int v6;

    v4 = 10 * a1 / 100;
    if (rate <= vol)
        v6 = dword_68D084[v4] * (vol - rate) / 100 + rate;
    else
        v6 = dword_68D058[v4] * (vol - rate) / 100 + rate;
    if (v4 == 10)
    {
        v5 = 0;
    }
    else if (rate <= vol)
    {
        v5 = dword_68D088[v4] * (vol - rate) / 100 + rate - v6;
    }
    else
    {
        v5 = dword_68D05C[v4] * (vol - rate) / 100 + rate - v6;
    }
    return (a1 - 100 * (10 * a1 / 100) / 10) * v5 / 10 + v6;
}

// 0x00523CB9
void __cdecl Sound_Unknown6()
{
    if (gSndBuffer_dword_77E0A0)
    {
        gSndBuffer_dword_77E0A0->SetVolume(dword_68CE18);
        gSndBuffer_dword_77E0A0->Play(0, 0, DSBPLAY_LOOPING);
    }
}