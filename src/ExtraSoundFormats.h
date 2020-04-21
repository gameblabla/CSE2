#pragma once

void ExtraSound_Init(unsigned int sample_rate);
void ExtraSound_Deinit(void);
void ExtraSound_Play(void);
void ExtraSound_Stop(void);
void ExtraSound_LoadMusic(const char *intro_file_path, const char *loop_file_path, bool loop);
void ExtraSound_LoadPreviousMusic(void);
void ExtraSound_PauseMusic(void);
void ExtraSound_FadeOutMusic(void);
void ExtraSound_LoadSFX(const char *path, int id);
void ExtraSound_PlaySFX(int id, int mode);
void ExtraSound_SetSFXFrequency(int id, unsigned long frequency);
void ExtraSound_Mix(float *buffer, unsigned long frames);
