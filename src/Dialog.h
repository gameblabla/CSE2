#pragma once

#include "WindowsWrapper.h"

DLGPROC_RET CALLBACK VersionDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DLGPROC_RET CALLBACK DebugMuteDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DLGPROC_RET CALLBACK DebugSaveDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DLGPROC_RET CALLBACK QuitDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
