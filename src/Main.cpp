#include <stddef.h>
#include "Types.h"
#include "CommonDefines.h"
#include <stdint.h>
#include <string>

#include <SDL.h>
#include "WindowsWrapper.h"

#include "Draw.h"
#include "Input.h"
#include "Profile.h"
#include "Organya.h"
#include "Sound.h"
#include "Game.h"
#include "Generic.h"
#include "MyChar.h"
#include "Main.h"
#include "Config.h"
#include "KeyControl.h"
#include "Triangle.h"
#include "Resource.h"

char gModulePath[PATH_LENGTH];
char gDataPath[PATH_LENGTH];

int gJoystickButtonTable[8];

int gWindowWidth;
int gWindowHeight;
int gWindowScale;
SDL_Window *gWindow;
SDL_Renderer *gRenderer;

bool gbUseJoystick = false;
bool bFullscreen = false;
bool bFps = false;

bool bActive = true;

#ifdef JAPANESE
const char *lpWindowName = "洞窟物語エンジン2";
#else
const char *lpWindowName = "Cave Story Engine 2 ~ Doukutsu Monogatari Enjin 2";
#endif

//A replication of MSVC's rand algorithm
static unsigned long int next = 1;

int rep_rand()
{
	next = ((next) * 214013 + 2531011);
    return ((next) >> 16) & 0x7FFF;
}

void rep_srand(unsigned int seed)
{
    next = seed;
}

//Framerate stuff
void PutFramePerSecound()
{
	if (bFps)
		PutNumber4(WINDOW_WIDTH - 40, 8, GetFramePerSecound(), false);
}

int GetFramePerSecound()
{
	unsigned int current_tick;
	static bool need_new_base_tick = true;
	static int frames_this_second;
	static int current_frame;
	static int base_tick;

	if (need_new_base_tick)
	{
		base_tick = SDL_GetTicks();
		need_new_base_tick = false;
	}
	
	current_tick = SDL_GetTicks();
	++current_frame;
	
	if ( base_tick + 1000 <= current_tick )
	{
		base_tick += 1000;
		frames_this_second = current_frame;
		current_frame = 0;
	}
	
	return frames_this_second;
}

int main(int argc, char *argv[])
{
	//Get executable's path
	strcpy(gModulePath, SDL_GetBasePath());
	if (gModulePath[strlen(gModulePath) - 1] == '/' || gModulePath[strlen(gModulePath) - 1] == '\\')
		gModulePath[strlen(gModulePath) - 1] = 0; //String cannot end in slash or stuff will probably break (original does this through a windows.h provided function)
	
	//Get path of the data folder
	strcpy(gDataPath, gModulePath);
	memcpy(&gDataPath[strlen(gDataPath)], "/data", 6); //Pixel didn't use a strcat

#ifdef WINDOWS
	// Set the window icons. See res/ICON/ICON.rc.
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "101");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "102");
#endif

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) >= 0)
	{
		//Load configuration
		CONFIG config;
		
		if (!LoadConfigData(&config))
			DefaultConfigData(&config);
		
		//Apply keybinds
		//Swap X and Z buttons
		if (config.attack_button_mode)
		{
			if (config.attack_button_mode == 1)
			{
				gKeyJump = KEY_X;
				gKeyShot = KEY_Z;
			}
		}
		else
		{
			gKeyJump = KEY_Z;
			gKeyShot = KEY_X;
		}
		
		//Swap Okay and Cancel buttons
		if (config.ok_button_mode)
		{
			if (config.ok_button_mode == 1)
			{
				gKeyOk = gKeyShot;
				gKeyCancel = gKeyJump;
			}
		}
		else
		{
			gKeyOk = gKeyJump;
			gKeyCancel = gKeyShot;
		}
		
		//Swap left and right weapon switch keys
		if (CheckFileExists("s_reverse"))
		{
			gKeyArms = KEY_ARMSREV;
			gKeyArmsRev = KEY_ARMS;
		}
		
		//Alternate movement keys
		if (config.move_button_mode)
		{
			if (config.move_button_mode == 1)
			{
				gKeyLeft = KEY_ALT_LEFT;
				gKeyUp = KEY_ALT_UP;
				gKeyRight = KEY_ALT_RIGHT;
				gKeyDown = KEY_ALT_DOWN;
			}
		}
		else
		{
			gKeyLeft = KEY_LEFT;
			gKeyUp = KEY_UP;
			gKeyRight = KEY_RIGHT;
			gKeyDown = KEY_DOWN;
		}
		
		//Set gamepad inputs
		for (int i = 0; i < 8; i++)
		{
			switch (config.joystick_button[i])
			{
				case 1:
					gJoystickButtonTable[i] = gKeyJump;
					break;
					
				case 2:
					gJoystickButtonTable[i] = gKeyShot;
					break;
					
				case 3:
					gJoystickButtonTable[i] = gKeyArms;
					break;
					
				case 6:
					gJoystickButtonTable[i] = gKeyArmsRev;
					break;
					
				case 4:
					gJoystickButtonTable[i] = gKeyItem;
					break;
					
				case 5:
					gJoystickButtonTable[i] = gKeyMap;
					break;
					
				default:
					continue;
			}
		}
		
		RECT unused_rect = {0, 0, 320, 240};

		//Load cursor
		SDL_RWops *fp = FindResource("CURSOR_NORMAL");
		
		if (fp)
		{
			SDL_Surface *cursor_surface = SDL_LoadBMP_RW(fp, 1);
			SDL_SetColorKey(cursor_surface, SDL_TRUE, SDL_MapRGB(cursor_surface->format, 0xFF, 0, 0xFF));	// Pink regions are transparent

			SDL_Cursor *cursor = SDL_CreateColorCursor(cursor_surface, 0, 0);	// Don't worry, the hotspots are accurate to the original files

			if (cursor)
				SDL_SetCursor(cursor);
			else
				printf("Failed to load cursor");

			SDL_FreeSurface(cursor_surface);
		}
		else
		{
			printf("Failed to load cursor");
		}

		//Get window dimensions and colour depth
		int colourDepth = 16;
		
		switch (config.display_mode)
		{
			case 1:
			case 2:
				//Set window dimensions
				if (config.display_mode == 1)
				{
					gWindowWidth = WINDOW_WIDTH;
					gWindowHeight = WINDOW_HEIGHT;
					gWindowScale = 1;
				}
				else
				{
					gWindowWidth = WINDOW_WIDTH * 2;
					gWindowHeight = WINDOW_HEIGHT * 2;
					gWindowScale = 2;
				}
				break;
			
			case 0:
			case 3:
			case 4:
				//Set window dimensions
				gWindowWidth = WINDOW_WIDTH * 2;
				gWindowHeight = WINDOW_HEIGHT * 2;
				gWindowScale = 2;
				
				//Set colour depth
				if (config.display_mode)
				{
					if (config.display_mode == 3)
						colourDepth = 24;
					else if (config.display_mode == 4)
						colourDepth = 32;
				}
				else
					colourDepth = 16;
				
				bFullscreen = true;
				SDL_ShowCursor(0);
				break;
		}
		
		//Create window
		gWindow = SDL_CreateWindow(lpWindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWindowWidth, gWindowHeight, bFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		
		if (gWindow)
		{
			//Initialize rendering
			StartDirectDraw();
			
			//Check debug things
			if (CheckFileExists("fps"))
				bFps = true;
			
#ifndef WINDOWS
			//Load icon
			SDL_RWops *fp = FindResource("ICON_MINI");
			
			if (fp)
			{
				SDL_Surface *iconSurf = SDL_LoadBMP_RW(fp, 1);
				SDL_Surface *iconConverted = SDL_ConvertSurfaceFormat(iconSurf, SDL_PIXELFORMAT_RGB888, 0);
				SDL_FreeSurface(iconSurf);
				SDL_Surface *iconSurfUpscaled = SDL_CreateRGBSurfaceWithFormat(0, 256, 256, 0, SDL_PIXELFORMAT_RGB888);
				SDL_LowerBlitScaled(iconConverted, NULL, iconSurfUpscaled, NULL);
				SDL_FreeSurface(iconConverted);
				SDL_SetWindowIcon(gWindow, iconSurfUpscaled);
				SDL_FreeSurface(iconSurfUpscaled);
			}
			else
			{
				printf("Failed to load icon");
			}
#endif

			//Set rects
			RECT loading_rect = {0, 0, 64, 8};
			RECT clip_rect = {0, 0, gWindowWidth, gWindowHeight};
			
			//Load the "LOADING" text
			MakeSurface_File("Loading", SURFACE_ID_LOADING);
			
			//Draw loading screen
			CortBox(&clip_rect, 0x000000);
			PutBitmap3(&clip_rect, (WINDOW_WIDTH - 64) / 2, (WINDOW_HEIGHT - 8) / 2, &loading_rect, SURFACE_ID_LOADING);
			
			//Draw to screen
			if (Flip_SystemTask())
			{
				//Initialize sound
				InitDirectSound();
				
				//Initialize joystick
				if (config.bJoystick && InitDirectInput())
				{
					ResetJoystickStatus();
					gbUseJoystick = true;
				}
				
				//Initialize stuff
				InitTextObject(config.font_name);
				InitTriangleTable();
				
				//Run game code
				Game();
				
				//End stuff
				EndDirectSound();
				EndTextObject();
				EndDirectDraw();
				
				SDL_Quit();
			}
		}
	}
	else
	{
		return -1;
	}
	
	return 0;
}

void InactiveWindow()
{
	if (bActive)
	{
		bActive = false;
		StopOrganyaMusic();
		SleepNoise();
	}
	
	PlaySoundObject(7, 0);
}

void ActiveWindow()
{
	if (!bActive)
	{
		bActive = true;
		StopOrganyaMusic();
		PlayOrganyaMusic();
		ResetNoise();
	}

	PlaySoundObject(7, -1);
}

void JoystickProc()
{
	JOYSTICK_STATUS status;
	
	if (GetJoystickStatus(&status))
	{
		//Clear held buttons
		gKey &= (KEY_ESCAPE | KEY_F2 | KEY_F1);
		
		//Set movement buttons
		if (status.bLeft)
			gKey |= gKeyLeft;
		if (status.bRight)
			gKey |= gKeyRight;
		if (status.bUp)
			gKey |= gKeyUp;
		if (status.bDown)
			gKey |= gKeyDown;
		
		//Set held buttons
		for (int i = 0; i < 8; i++)
		{
			if (status.bButton[i])
				gKey |= gJoystickButtonTable[i];
		}
	}
}

bool SystemTask()
{
	//Handle window events
	bool focusGained = true;
	
	while (SDL_PollEvent(nullptr) || !focusGained)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);
		
		switch (event.type)
		{
			case SDL_QUIT:
				return false;
				break;
				
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						focusGained = true;
						ActiveWindow();
						break;
						
					case SDL_WINDOWEVENT_FOCUS_LOST:
						focusGained = false;
						InactiveWindow();
						break;
					
					default:
						break;
				}
				break;
				
			case SDL_DROPFILE:
				LoadProfile(event.drop.file);
				SDL_free(event.drop.file);
				break;
			
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						gKey |= KEY_ESCAPE;
						break;
						
					case SDLK_w:
						gKey |= KEY_MAP;
						break;
						
					case SDLK_LEFT:
						gKey |= KEY_LEFT;
						break;
						
					case SDLK_RIGHT:
						gKey |= KEY_RIGHT;
						break;
						
					case SDLK_UP:
						gKey |= KEY_UP;
						break;
						
					case SDLK_DOWN:
						gKey |= KEY_DOWN;
						break;
						
					case SDLK_x:
						gKey |= KEY_X;
						break;
						
					case SDLK_z:
						gKey |= KEY_Z;
						break;
						
					case SDLK_s:
						gKey |= KEY_ARMS;
						break;
						
					case SDLK_a:
						gKey |= KEY_ARMSREV;
						break;
					
					case SDLK_RSHIFT:
					case SDLK_LSHIFT:
						gKey |= KEY_SHIFT;
						break;
						
					case SDLK_F1:
						gKey |= KEY_F1;
						break;
						
					case SDLK_F2:
						gKey |= KEY_F2;
						break;
						
					case SDLK_q:
						gKey |= KEY_ITEM;
						break;
						
					case SDLK_COMMA:
						gKey |= KEY_ALT_LEFT;
						break;
						
					case SDLK_PERIOD:
						gKey |= KEY_ALT_DOWN;
						break;
						
					case SDLK_SLASH:
						gKey |= KEY_ALT_RIGHT;
						break;
						
					case SDLK_l:
						gKey |= KEY_ALT_UP;
						break;
						
				#ifdef FIX_BUGS //BUG FIX: Pixel intended for the second alternate up key to be the plus key, Japanese keyboards have the plus key where the semi-colon key is, causing errors on other keyboard layouts)
					case SDLK_PLUS:
				#else
					case SDLK_SEMICOLON:
				#endif
						gKey |= KEY_PLUS;
						break;
						
					case SDLK_F5:
						gbUseJoystick = false;
						break;
						
					default:
						break;
				}
				break;
				
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						gKey &= ~KEY_ESCAPE;
						break;
						
					case SDLK_w:
						gKey &= ~KEY_MAP;
						break;
						
					case SDLK_LEFT:
						gKey &= ~KEY_LEFT;
						break;
						
					case SDLK_RIGHT:
						gKey &= ~KEY_RIGHT;
						break;
						
					case SDLK_UP:
						gKey &= ~KEY_UP;
						break;
						
					case SDLK_DOWN:
						gKey &= ~KEY_DOWN;
						break;
						
					case SDLK_x:
						gKey &= ~KEY_X;
						break;
						
					case SDLK_z:
						gKey &= ~KEY_Z;
						break;
						
					case SDLK_s:
						gKey &= ~KEY_ARMS;
						break;
						
					case SDLK_a:
						gKey &= ~KEY_ARMSREV;
						break;
					
					case SDLK_RSHIFT:
					case SDLK_LSHIFT:
						gKey &= ~KEY_SHIFT;
						break;
						
					case SDLK_F1:
						gKey &= ~KEY_F1;
						break;
						
					case SDLK_F2:
						gKey &= ~KEY_F2;
						break;
						
					case SDLK_q:
						gKey &= ~KEY_ITEM;
						break;
						
					case SDLK_COMMA:
						gKey &= ~KEY_ALT_LEFT;
						break;
						
					case SDLK_PERIOD:
						gKey &= ~KEY_ALT_DOWN;
						break;
						
					case SDLK_SLASH:
						gKey &= ~KEY_ALT_RIGHT;
						break;
						
					case SDLK_l:
						gKey &= ~KEY_ALT_UP;
						break;
						
					#ifdef FIX_BUGS //BUG FIX: Pixel intended for the second alternate up key to be the plus key, Japanese keyboards have the plus key where the semi-colon key is, causing errors on other keyboard layouts)
						case SDLK_PLUS:
					#else
						case SDLK_SEMICOLON:
					#endif
						gKey &= ~KEY_PLUS;
						break;
						
					default:
						break;
				}
				break;
		}
	}
	
	//Run joystick code
	if (gbUseJoystick)
		JoystickProc();
	
	return true;
}
