#pragma once

// Visual Studio 6 is missing these, so define them here
#ifdef _MSC_VER
	#ifndef VK_OEM_PLUS
	#define VK_OEM_PLUS 0xBB
	#endif

	#ifndef VK_OEM_COMMA
	#define VK_OEM_COMMA 0xBC
	#endif

	#ifndef VK_OEM_PERIOD
	#define VK_OEM_PERIOD 0xBE
	#endif

	#ifndef VK_OEM_2
	#define VK_OEM_2 0xBF
	#endif

	#ifndef DWORD_PTR
	#define DWORD_PTR DWORD
	#endif
#endif

#include <windows.h>
