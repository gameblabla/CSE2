#pragma once

#include "WindowsWrapper.h"

BOOL __stdcall VersionDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL __stdcall DebugMuteDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL __stdcall DebugSaveDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
BOOL __stdcall QuitDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
