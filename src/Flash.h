#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void InitFlash(void);
void SetFlash(int x, int y, int mode);
void ActFlash_Explosion(int flx, int fly);
void ActFlash_Flash(void);
void ActFlash(int flx, int fly);
void PutFlash(void);
void ResetFlash(void);

#ifdef __cplusplus
}
#endif
