/*
 *  (C) 2020 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include "clownaudio_export.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ClownAudio_SoundDataConfig
{
	bool predecode;
	bool must_predecode;
	bool dynamic_sample_rate;
} ClownAudio_SoundDataConfig;

typedef struct ClownAudio_SoundConfig
{
	bool loop;
	bool do_not_free_when_done;
	bool dynamic_sample_rate;
} ClownAudio_SoundConfig;

CLOWNAUDIO_EXPORT void ClownAudio_InitSoundDataConfig(ClownAudio_SoundDataConfig *config);
CLOWNAUDIO_EXPORT void ClownAudio_InitSoundConfig(ClownAudio_SoundConfig *config);

#ifdef __cplusplus
}
#endif
