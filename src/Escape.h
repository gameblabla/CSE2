#pragma once

#include "WindowsWrapper.h"

enum enum_ESCRETURN
{
	enum_ESCRETURN_exit,
	enum_ESCRETURN_continue,
	enum_ESCRETURN_restart
};

int Call_Escape(HWND hWnd);
