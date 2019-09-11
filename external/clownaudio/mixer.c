#include "mixer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "miniaudio.h"

#include "decoder.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct Channel
{
	struct Channel *next;

	bool paused;
	float volume;
	Decoder *decoder;
	ma_pcm_converter dsp;
	Mixer_SoundInstanceID instance;

	unsigned int fade_out_counter_max;
	unsigned int fade_in_counter_max;
	unsigned int fade_counter;
} Channel;

typedef struct Mutex
{
#ifdef _WIN32
	HANDLE handle;
#else
	pthread_mutex_t pthread_mutex;
#endif
} Mutex;

static Channel *channel_list_head;

static Mutex mixer_mutex;

static unsigned int output_sample_rate;
static unsigned int output_channel_count;

static void MutexInit(Mutex *mutex)
{
#ifdef _WIN32
	mutex->handle = CreateEventA(NULL, FALSE, TRUE, NULL);
#else
	pthread_mutex_init(&mutex->pthread_mutex, NULL);
#endif
}

static void MutexDeinit(Mutex *mutex)
{
#ifdef _WIN32
	CloseHandle(mutex->handle);
#else
	pthread_mutex_destroy(&mutex->pthread_mutex);
#endif
}

static void MutexLock(Mutex *mutex)
{
#ifdef _WIN32
	WaitForSingleObject(mutex->handle, INFINITE);
#else
	pthread_mutex_lock(&mutex->pthread_mutex);
#endif
}

static void MutexUnlock(Mutex *mutex)
{
#ifdef _WIN32
	SetEvent(mutex->handle);
#else
	pthread_mutex_unlock(&mutex->pthread_mutex);
#endif
}

static Channel* FindChannel(Mixer_SoundInstanceID instance)
{
	for (Channel *channel = channel_list_head; channel != NULL; channel = channel->next)
		if (channel->instance == instance)
			return channel;

	return NULL;
}

static ma_uint32 CallbackDSP(ma_pcm_converter *dsp, void *output_buffer, ma_uint32 frames_to_do, void *user_data)
{
	(void)dsp;

	return Decoder_GetSamples((Decoder*)user_data, output_buffer, frames_to_do);
}

void Mixer_Init(unsigned int sample_rate, unsigned int channel_count)
{
	output_sample_rate = sample_rate;
	output_channel_count = channel_count;

	MutexInit(&mixer_mutex);
}

void Mixer_Deinit(void)
{
	MutexDeinit(&mixer_mutex);
}

Mixer_Sound* Mixer_LoadSound(const char *file_path, bool predecode)
{
	return (Mixer_Sound*)Decoder_LoadData(file_path, predecode);
}

void Mixer_UnloadSound(Mixer_Sound *sound)
{
	Decoder_UnloadData((DecoderData*)sound);
}

Mixer_SoundInstanceID Mixer_PlaySound(Mixer_Sound *sound, bool loop)
{
	static Mixer_SoundInstanceID instance_allocator;

	Mixer_SoundInstanceID instance = 0;

	DecoderInfo info;
	Decoder *decoder = Decoder_Create((DecoderData*)sound, loop, &info);

	if (decoder)
	{
		instance = ++instance_allocator;

		Channel *channel = malloc(sizeof(Channel));

		channel->decoder = decoder;
		channel->volume = 1.0f;
		channel->instance = instance;
		channel->paused = true;
		channel->fade_out_counter_max = 0;
		channel->fade_in_counter_max = 0;

		ma_format format;
		if (info.format == DECODER_FORMAT_S16)
			format = ma_format_s16;
		else if (info.format == DECODER_FORMAT_S32)
			format = ma_format_s32;
		else //if (info.format == DECODER_FORMAT_F32)
			format = ma_format_f32;

		const ma_pcm_converter_config config = ma_pcm_converter_config_init(format, info.channel_count, info.sample_rate, ma_format_f32, output_channel_count, output_sample_rate, CallbackDSP, channel->decoder);
		ma_pcm_converter_init(&config, &channel->dsp);

		MutexLock(&mixer_mutex);
		channel->next = channel_list_head;
		channel_list_head = channel;
		MutexUnlock(&mixer_mutex);
	}

	return instance;
}

void Mixer_StopSound(Mixer_SoundInstanceID instance)
{
	Channel *channel = NULL;

	MutexLock(&mixer_mutex);

	for (Channel **channel_pointer = &channel_list_head; *channel_pointer != NULL; channel_pointer = &(*channel_pointer)->next)
	{
		if ((*channel_pointer)->instance == instance)
		{
			channel = *channel_pointer;
			*channel_pointer = channel->next;
			break;
		}
	}

	MutexUnlock(&mixer_mutex);

	if (channel)
	{
		Decoder_Destroy(channel->decoder);
		free(channel);
	}
}

void Mixer_PauseSound(Mixer_SoundInstanceID instance)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
		channel->paused = true;

	MutexUnlock(&mixer_mutex);
}

void Mixer_UnpauseSound(Mixer_SoundInstanceID instance)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
		channel->paused = false;

	MutexUnlock(&mixer_mutex);
}

void Mixer_FadeOutSound(Mixer_SoundInstanceID instance, unsigned int duration)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
	{
		channel->fade_out_counter_max = (output_sample_rate * duration) / 1000;
		channel->fade_counter = channel->fade_in_counter_max ? (((unsigned long long)channel->fade_in_counter_max - channel->fade_counter) * channel->fade_out_counter_max) / channel->fade_in_counter_max : channel->fade_out_counter_max;
		channel->fade_in_counter_max = 0;
	}

	MutexUnlock(&mixer_mutex);
}

void Mixer_FadeInSound(Mixer_SoundInstanceID instance, unsigned int duration)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
	{
		channel->fade_in_counter_max = (output_sample_rate * duration) / 1000;
		channel->fade_counter = channel->fade_out_counter_max ? (((unsigned long long)channel->fade_out_counter_max - channel->fade_counter) * channel->fade_in_counter_max) / channel->fade_out_counter_max : channel->fade_in_counter_max;
		channel->fade_out_counter_max = 0;
	}

	MutexUnlock(&mixer_mutex);
}

void Mixer_CancelFade(Mixer_SoundInstanceID instance)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
	{
		channel->fade_in_counter_max = 0;
		channel->fade_out_counter_max = 0;
	}

	MutexUnlock(&mixer_mutex);
}

void Mixer_SetSoundVolume(Mixer_SoundInstanceID instance, float volume)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel)
		channel->volume = volume * volume;

	MutexUnlock(&mixer_mutex);
}

void Mixer_MixSamples(float *output_buffer, unsigned long frames_to_do)
{
	MutexLock(&mixer_mutex);
	Channel **channel_pointer = &channel_list_head;
	while (*channel_pointer != NULL)
	{
		Channel *channel = *channel_pointer;

		if (channel->paused == false)
		{
			float *output_buffer_pointer = output_buffer;

			unsigned long frames_done = 0;
			for (unsigned long sub_frames_done; frames_done < frames_to_do; frames_done += sub_frames_done)
			{
				float read_buffer[0x1000];

				const unsigned long sub_frames_to_do = MIN(0x1000 / output_channel_count, frames_to_do - frames_done);
				sub_frames_done = (unsigned long)ma_pcm_converter_read(&channel->dsp, read_buffer, sub_frames_to_do);

				float *read_buffer_pointer = read_buffer;

				for (unsigned long i = 0; i < sub_frames_done; ++i)
				{
					float volume = channel->volume;

					if (channel->fade_out_counter_max)
					{
						const float fade_out_volume = channel->fade_counter / (float)channel->fade_out_counter_max;

						volume *= (fade_out_volume * fade_out_volume);

						if (channel->fade_counter)
							--channel->fade_counter;
					}

					if (channel->fade_in_counter_max)
					{
						const float fade_in_volume = (channel->fade_in_counter_max - channel->fade_counter) / (float)channel->fade_in_counter_max;

						volume *= (fade_in_volume * fade_in_volume);

						if (--channel->fade_counter == 0)
							channel->fade_in_counter_max = 0;
					}

					for (unsigned int j = 0; j < output_channel_count; ++j)
						*output_buffer_pointer++ += *read_buffer_pointer++ * volume;
				}

				if (sub_frames_done < sub_frames_to_do)
				{
					frames_done += sub_frames_done;
					break;
				}
			}

			if (frames_done < frames_to_do)	// Sound finished
			{
				Decoder_Destroy(channel->decoder);
				*channel_pointer = channel->next;
				free(channel);
				continue;
			}
		}

		channel_pointer = &(*channel_pointer)->next;
	}
	MutexUnlock(&mixer_mutex);
}
