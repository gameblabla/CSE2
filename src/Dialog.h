#pragma once

#include "WindowsWrapper.h"

INT_PTR __stdcall VersionDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall DebugMuteDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall DebugSaveDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall QuitDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
