#include <stddef.h>
#include "Types.h"
#include "CommonDefines.h"
#include <stdint.h>
#include <string>

#include <SDL.h>
#include "WindowsWrapper.h"

#include "Input.h"
#include "Config.h"
#include "KeyControl.h"

char gModulePath[PATH_LENGTH];
char gDataPath[PATH_LENGTH];

int gJoystickButtonTable[8];

int gWindowWidth;
int gWindowHeight;
SDL_Window *gWindow;
SDL_Renderer *gRenderer;

bool gbUseJoystick;
bool bFullscreen;
bool bFps;

bool bActive;

int main(int argc, char *argv[])
{
	//Get executable's path
	strcpy(gModulePath, SDL_GetBasePath());
	if (gModulePath[strlen(gModulePath) - 1] == '/' || gModulePath[strlen(gModulePath) - 1] == '\\')
		gModulePath[strlen(gModulePath) - 1] = 0; //String cannot end in slash or stuff will probably break (original does this through a windows.h provided function)
	
	//Get path of the data folder
	strcpy(gDataPath, gModulePath);
	memcpy(&gDataPath[strlen(gDataPath)], "/data", 6); //Pixel didn't use a strcat
	
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) >= 0)
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
		
		//Get window dimensions and colour depth
		int windowScale;
		int colourDepth = 16;
		
		switch (config.display_mode)
		{
			case 1:
			case 2:
				//Set window dimensions
				if (config.display_mode == 1)
				{
					gWindowWidth = 320;
					gWindowHeight = 240;
					windowScale = 1;
				}
				else
				{
					gWindowWidth = 640;
					gWindowHeight = 480;
					windowScale = 2;
				}
				break;
			
			case 0:
			case 3:
			case 4:
				//Set window dimensions
				gWindowWidth = 640;
				gWindowHeight = 480;
				windowScale = 2;
				
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
		
		#ifdef JAPANESE
		const char *windowTitle = "洞窟物語エンジン";
		#else
		const char *windowTitle = "Cave Story Engine ~ Doukutsu Monogatari Enjin";
		#endif
		
		gWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWindowWidth, gWindowHeight, bFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		
		if (gWindow)
		{
			StartDirectDraw(window, windowScale);
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
		//StopOrganyaMusic();
		//SleepNoise();
	}
	
	//PlaySoundObject(7, 0);
}

void ActiveWindow()
{
	if (!bActive)
	{
		bActive = true;
		//StopOrganyaMusic();
		//PlayOrganyaMusic();
		//ResetNoise();
	}

	//PlaySoundObject(7, -1);
}

void JoystickProc()
{
	JOYSTICK_STATUS status;
	
	if (GetJoystickStatus(&status))
	{
		//Set movement buttons
		if (status.bLeft)
			gKey |= gKeyLeft;
		if (status.bRight)
			gKey |= gKeyRight;
		if (status.bUp)
			gKey |= gKeyUp;
		if (status.bDown)
			gKey |= gKeyDown;
		
		//Clear previously held buttons
		for (int i = 0; i < 8; i++)
			gKey &= ~gJoystickButtonTable[i];
			
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
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return false;
				break;
				
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						ActiveWindow();
						break;
						
					case SDL_WINDOWEVENT_FOCUS_LOST:
						InactiveWindow();
						break;
					
					default:
						break;
				}
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
						return true;
				}
				break;
				
			case SDL_KEYUP:
				switch (event.key.keysym.scancode)
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
						return true;
				}
				break;
		}
	}
	
	//Run joystick code
	if (gbUseJoystick)
		JoystickProc();
	
	return true;
}
