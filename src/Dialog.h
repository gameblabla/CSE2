#pragma once

#include "WindowsWrapper.h"

INT_PTR CALLBACK VersionDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DebugMuteDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DebugSaveDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK QuitDialog(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
