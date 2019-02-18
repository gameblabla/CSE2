#pragma once

#include "WindowsWrapper.h"

struct VALUEVIEW
{
  int flag;
  int *px;
  int *py;
  int offset_y;
  int value;
  int count;
  RECT rect;
};

void ClearValueView();
void SetValueView(int *px, int *py, int value);
void ActValueView();
void PutValueView(int flx, int fly);
