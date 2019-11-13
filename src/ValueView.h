#pragma once

#include "WindowsWrapper.h"

struct VALUEVIEW
{
  BOOL flag;
  int *px;
  int *py;
  int offset_y;
  int value;
  int count;
  RECT rect;
};

void ClearValueView(void);
void SetValueView(int *px, int *py, int value);
void ActValueView(void);
void PutValueView(int flx, int fly);
