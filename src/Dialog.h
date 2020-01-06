#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

INT_PTR __stdcall VersionDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall DebugMuteDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall DebugSaveDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall QuitDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif
