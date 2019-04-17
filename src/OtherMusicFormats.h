#pragma once

#include <stdbool.h>

void OtherMusic_Init(unsigned int sample_rate, unsigned int channel_count);
void OtherMusic_Deinit(void);
void OtherMusic_Play(void);
void OtherMusic_Stop(void);
void OtherMusic_Load(const char *path, bool loop);
void OtherMusic_LoadPrevious(void);
void OtherMusic_Mix(float (*buffer)[2], unsigned long frames);
