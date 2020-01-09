#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

#include "Config.h"
#include "File.h"
#include "Main.h"

static const char* const config_filename = "Config.dat";	// Not the original name
static const char* const config_magic = "CSE2E   20200104";	// Not the original name

BOOL LoadConfigData(CONFIG *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIG));

	// Get path
	char path[MAX_PATH];
	sprintf(path, "%s/%s", gModulePath, config_filename);

	// Open file
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	// Read the version id and font name
	fread(conf->proof, sizeof(conf->proof), 1, fp);
	fread(conf->font_name, sizeof(conf->font_name), 1, fp);

	// Read display mode (320x240, 640x480, 24-bit fullscreen, 32-bit fullscreen) TODO: add more things?
	conf->display_mode = fgetc(fp);

	// Read 'joystick enabled' flag
	conf->bJoystick = fgetc(fp);

	// Read framerate toggle
	conf->b60fps = fgetc(fp);

	// Read vsync toggle
	conf->bVsync = fgetc(fp);

	// Read key-bindings
	for (size_t i = 0; i < BINDING_TOTAL; ++i)
	{
		conf->bindings[i].keyboard = File_ReadLE32(fp);
		conf->bindings[i].controller = fgetc(fp);
	}

	// Close file
	fclose(fp);

	// Check if version is not correct, and return if it failed
	if (strcmp(conf->proof, config_magic))
	{
		memset(conf, 0, sizeof(CONFIG));
		return FALSE;
	}

	return TRUE;
}

BOOL SaveConfigData(const CONFIG *conf)
{
	// Get path
	char path[MAX_PATH];
	sprintf(path, "%s/%s", gModulePath, config_filename);

	// Open file
	FILE *fp = fopen(path, "wb");
	if (fp == NULL)
		return FALSE;

	// Read the version id and font name
	fwrite(conf->proof, sizeof(conf->proof), 1, fp);
	fwrite(conf->font_name, sizeof(conf->font_name), 1, fp);

	// Read display mode (320x240, 640x480, 24-bit fullscreen, 32-bit fullscreen) TODO: add more things?
	fputc(conf->display_mode, fp);

	// Read 'joystick enabled' flag
	fputc(conf->bJoystick, fp);

	// Read framerate toggle
	fputc(conf->b60fps, fp);

	// Read vsync toggle
	fputc(conf->bVsync, fp);

	// Read key-bindings
	for (size_t i = 0; i < BINDING_TOTAL; ++i)
	{
		File_WriteLE32(conf->bindings[i].keyboard, fp);
		fputc(conf->bindings[i].controller, fp);
	}

	// Close file
	fclose(fp);

	return TRUE;
}

void DefaultConfigData(CONFIG *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIG));

	strcpy(conf->proof, config_magic);

	// Fun fact: The Linux port added this line:
	// conf->display_mode = 1;

	// Reset joystick settings (as these can't simply be set to 0)
	conf->bJoystick = TRUE;
	conf->bindings[BINDING_UP].controller = 0xFF;
	conf->bindings[BINDING_DOWN].controller = 0xFF;
	conf->bindings[BINDING_LEFT].controller = 0xFF;
	conf->bindings[BINDING_RIGHT].controller = 0xFF;
	conf->bindings[BINDING_OK].controller = 1;
	conf->bindings[BINDING_CANCEL].controller = 0;
	conf->bindings[BINDING_JUMP].controller = 1;
	conf->bindings[BINDING_SHOT].controller = 0;
	conf->bindings[BINDING_ARMSREV].controller = 3;
	conf->bindings[BINDING_ARMS].controller = 4;
	conf->bindings[BINDING_ITEM].controller = 5;
	conf->bindings[BINDING_MAP].controller = 2;
	conf->bindings[BINDING_PAUSE].controller = 0xFF;

	// Set default key bindings
	conf->bindings[BINDING_UP].keyboard = SDL_SCANCODE_UP;
	conf->bindings[BINDING_DOWN].keyboard = SDL_SCANCODE_DOWN;
	conf->bindings[BINDING_LEFT].keyboard = SDL_SCANCODE_LEFT;
	conf->bindings[BINDING_RIGHT].keyboard = SDL_SCANCODE_RIGHT;
	conf->bindings[BINDING_OK].keyboard = SDL_SCANCODE_Z;
	conf->bindings[BINDING_CANCEL].keyboard = SDL_SCANCODE_X;
	conf->bindings[BINDING_JUMP].keyboard = SDL_SCANCODE_Z;
	conf->bindings[BINDING_SHOT].keyboard = SDL_SCANCODE_X;
	conf->bindings[BINDING_ARMSREV].keyboard = SDL_SCANCODE_S;
	conf->bindings[BINDING_ARMS].keyboard = SDL_SCANCODE_A;
	conf->bindings[BINDING_ITEM].keyboard = SDL_SCANCODE_Q;
	conf->bindings[BINDING_MAP].keyboard = SDL_SCANCODE_W;
	conf->bindings[BINDING_PAUSE].keyboard = SDL_SCANCODE_ESCAPE;
}
