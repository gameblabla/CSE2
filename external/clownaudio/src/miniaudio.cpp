#define MINIAUDIO_IMPLEMENTATION

#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_GENERATION

#ifndef MINIAUDIO_ENABLE_DEVICE_IO
 #define MA_NO_DEVICE_IO
 #define MA_NO_THREADING
#else
 //#define MA_NO_WASAPI
 //#define MA_NO_DSOUND
 //#define MA_NO_WINMM
 //#define MA_NO_ALSA
 //#define MA_NO_PULSEAUDIO
 //#define MA_NO_JACK
 //#define MA_NO_COREAUDIO
 //#define MA_NO_SNDIO
 //#define MA_NO_AUDIO4
 //#define MA_NO_OSS
 //#define MA_NO_AAUDIO
 //#define MA_NO_OPENSL
 //#define MA_NO_WEBAUDIO
 //#define MA_NO_NULL
#endif

#include "miniaudio.h"
