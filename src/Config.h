#pragma once

#include "WindowsWrapper.h"

#include "Input.h"

enum
{
	BINDING_UP,
	BINDING_DOWN,
	BINDING_LEFT,
	BINDING_RIGHT,
	BINDING_OK,
	BINDING_CANCEL,
	BINDING_JUMP,
	BINDING_SHOT,
	BINDING_ARMSREV,
	BINDING_ARMS,
	BINDING_ITEM,
	BINDING_MAP,
	BINDING_PAUSE,
	BINDING_TOTAL
};

typedef struct CONFIG_BINDING
{
	int keyboard;
	unsigned char controller;
} CONFIG_BINDING;

struct CONFIG
{
	char proof[0x20];
	char font_name[0x40];
	unsigned char display_mode;
	BOOL b60fps;
	BOOL bVsync;
	BOOL bSmoothScrolling;
	unsigned char soundtrack;
	CONFIG_BINDING bindings[BINDING_TOTAL];
};

extern const char* const gConfigName;
extern const char* const gProof;

BOOL LoadConfigData(CONFIG *conf);
BOOL SaveConfigData(const CONFIG *conf);
void DefaultConfigData(CONFIG *conf);
