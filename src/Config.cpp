#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

#include "Config.h"
#include "File.h"
#include "Tags.h"

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
	for (size_t i = 0; i < sizeof(conf->key_bindings) / sizeof(conf->key_bindings[0]); ++i)
		conf->key_bindings[i] = File_ReadLE32(fp);

	// Read button-bindings
	for (size_t i = 0; i < sizeof(conf->button_bindings) / sizeof(conf->button_bindings[0]); ++i)
		conf->button_bindings[i] = fgetc(fp);

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
	for (size_t i = 0; i < sizeof(conf->key_bindings) / sizeof(conf->key_bindings[0]); ++i)
		File_WriteLE32(conf->key_bindings[i], fp);

	// Read button-bindings
	for (size_t i = 0; i < sizeof(conf->button_bindings) / sizeof(conf->button_bindings[0]); ++i)
		fputc(conf->button_bindings[i], fp);

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

	conf->bJoystick = TRUE;
/*	conf->joystick_button[0] = 2;
	conf->joystick_button[1] = 1;
	conf->joystick_button[2] = 5;
	conf->joystick_button[3] = 6;
	conf->joystick_button[4] = 3;
	conf->joystick_button[5] = 4;
	conf->joystick_button[6] = 6;
	conf->joystick_button[7] = 3;
*/

	// Set default key bindings too
	conf->key_bindings[0] = SDL_SCANCODE_UP;
	conf->key_bindings[1] = SDL_SCANCODE_DOWN;
	conf->key_bindings[2] = SDL_SCANCODE_LEFT;
	conf->key_bindings[3] = SDL_SCANCODE_RIGHT;
	conf->key_bindings[4] = SDL_SCANCODE_Z;
	conf->key_bindings[5] = SDL_SCANCODE_X;
	conf->key_bindings[6] = SDL_SCANCODE_Z;
	conf->key_bindings[7] = SDL_SCANCODE_X;
	conf->key_bindings[8] = SDL_SCANCODE_S;
	conf->key_bindings[9] = SDL_SCANCODE_A;
	conf->key_bindings[10] = SDL_SCANCODE_Q;
	conf->key_bindings[11] = SDL_SCANCODE_W;
	conf->key_bindings[12] = SDL_SCANCODE_ESCAPE;
}
