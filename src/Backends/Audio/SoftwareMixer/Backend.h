// Released under the MIT licence.
// See LICENCE.txt for details.

#pragma once

#include <stddef.h>

unsigned long SoftwareMixerBackend_Init(void (*callback)(long *stream, size_t frames_total));
void SoftwareMixerBackend_Deinit(void);

bool SoftwareMixerBackend_Start(void);

void SoftwareMixerBackend_LockMixerMutex(void);
void SoftwareMixerBackend_UnlockMixerMutex(void);

void SoftwareMixerBackend_LockOrganyaMutex(void);
void SoftwareMixerBackend_UnlockOrganyaMutex(void);
