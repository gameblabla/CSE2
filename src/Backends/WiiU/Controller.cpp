#include "../Controller.h"

#include <string.h>

#include <padscore/kpad.h>
#include <vpad/input.h>

bool ControllerBackend_Init(void)
{
	VPADInit();

	WPADInit();
	KPADInit();

	// Enable Wii U Pro Controllers to be connected
	WPADEnableURCC(1);

	return true;
}

void ControllerBackend_Deinit(void)
{
	WPADShutdown();

	VPADShutdown();
}

bool ControllerBackend_GetJoystickStatus(bool **buttons, unsigned int *button_count, short **axes, unsigned int *axis_count)
{
	static bool button_buffer[27];
	static short axis_buffer[4];

	*buttons = button_buffer;
	*axes = axis_buffer;

	*button_count = sizeof(button_buffer) / sizeof(button_buffer[0]);
	*axis_count = sizeof(axis_buffer) / sizeof(axis_buffer[0]);

	memset(button_buffer, 0, sizeof(button_buffer));
	memset(axis_buffer, 0, sizeof(axis_buffer));

	//////////////////
	// Read gamepad //
	//////////////////

	static uint32_t vpad_buttons;

	VPADStatus vpad_status;
	if (VPADRead(VPAD_CHAN_0, &vpad_status, 1, NULL) == 1)
		vpad_buttons = vpad_status.hold;

	// Handle button inputs
	button_buffer[0] |= vpad_buttons & VPAD_BUTTON_Y;     // Shoot
	button_buffer[1] |= vpad_buttons & VPAD_BUTTON_B;     // Jump
	button_buffer[2] |= vpad_buttons & VPAD_BUTTON_X;     // Map
	button_buffer[3] |= vpad_buttons & VPAD_BUTTON_ZL;    // Weapon left
	button_buffer[4] |= vpad_buttons & VPAD_BUTTON_ZR;    // Weapon right
	button_buffer[5] |= vpad_buttons & VPAD_BUTTON_A;     // Inventory
	button_buffer[6] |= vpad_buttons & VPAD_BUTTON_PLUS;  // Pause
	button_buffer[7] |= vpad_buttons & VPAD_STICK_L_EMULATION_UP;    // Up
	button_buffer[8] |= vpad_buttons & VPAD_STICK_L_EMULATION_DOWN;  // Down
	button_buffer[9] |= vpad_buttons & VPAD_STICK_L_EMULATION_LEFT;  // Left
	button_buffer[10] |= vpad_buttons & VPAD_STICK_L_EMULATION_RIGHT;// Right
	button_buffer[11] |= vpad_buttons & VPAD_BUTTON_L;
	button_buffer[12] |= vpad_buttons & VPAD_BUTTON_R;
	button_buffer[13] |= vpad_buttons & VPAD_BUTTON_MINUS;
	button_buffer[14] |= vpad_buttons & VPAD_BUTTON_HOME;
	button_buffer[15] |= vpad_buttons & VPAD_BUTTON_SYNC;
	button_buffer[16] |= vpad_buttons & VPAD_BUTTON_STICK_L;
	button_buffer[17] |= vpad_buttons & VPAD_BUTTON_STICK_R;
	button_buffer[18] |= vpad_buttons & VPAD_BUTTON_TV;
	button_buffer[19] |= vpad_buttons & VPAD_BUTTON_LEFT;
	button_buffer[20] |= vpad_buttons & VPAD_BUTTON_RIGHT;
	button_buffer[21] |= vpad_buttons & VPAD_BUTTON_UP;
	button_buffer[22] |= vpad_buttons & VPAD_BUTTON_DOWN;
	button_buffer[23] |= vpad_buttons & VPAD_STICK_R_EMULATION_LEFT;
	button_buffer[24] |= vpad_buttons & VPAD_STICK_R_EMULATION_RIGHT;
	button_buffer[25] |= vpad_buttons & VPAD_STICK_R_EMULATION_UP;
	button_buffer[26] |= vpad_buttons & VPAD_STICK_R_EMULATION_DOWN;

	/*
	// Handle axis inputs
	axis_buffer[0] = (short)(vpad_status.leftStick.x * 0x7FFF);
	axis_buffer[1] = (short)(vpad_status.leftStick.y * -0x7FFF);
	axis_buffer[2] = (short)(vpad_status.rightStick.x * 0x7FFF);
	axis_buffer[3] = (short)(vpad_status.rightStick.y * -0x7FFF);
	*/

	///////////////////////////////
	// Read Wii U Pro Controller //
	///////////////////////////////
	static uint32_t kpad_buttons;

	KPADStatus kpad_status;
	if (KPADRead(WPAD_CHAN_0, &kpad_status, 1) == 1)
		kpad_buttons = kpad_status.pro.hold;

	// Handle button inputs
	button_buffer[0] |= kpad_buttons & WPAD_PRO_BUTTON_Y;     // Shoot
	button_buffer[1] |= kpad_buttons & WPAD_PRO_BUTTON_B;     // Jump
	button_buffer[2] |= kpad_buttons & WPAD_PRO_BUTTON_X;     // Map
	button_buffer[3] |= kpad_buttons & WPAD_PRO_TRIGGER_ZL;    // Weapon left
	button_buffer[4] |= kpad_buttons & WPAD_PRO_TRIGGER_ZR;    // Weapon right
	button_buffer[5] |= kpad_buttons & WPAD_PRO_BUTTON_A;     // Inventory
	button_buffer[6] |= kpad_buttons & WPAD_PRO_BUTTON_PLUS;  // Pause
	button_buffer[7] |= kpad_buttons & WPAD_PRO_STICK_L_EMULATION_UP;    // Up
	button_buffer[8] |= kpad_buttons & WPAD_PRO_STICK_L_EMULATION_DOWN;  // Down
	button_buffer[9] |= kpad_buttons & WPAD_PRO_STICK_L_EMULATION_LEFT;  // Left
	button_buffer[10] |= kpad_buttons & WPAD_PRO_STICK_L_EMULATION_RIGHT;// Right
	button_buffer[11] |= kpad_buttons & WPAD_PRO_TRIGGER_L;
	button_buffer[12] |= kpad_buttons & WPAD_PRO_TRIGGER_R;
	button_buffer[13] |= kpad_buttons & WPAD_PRO_BUTTON_MINUS;
	button_buffer[14] |= kpad_buttons & WPAD_PRO_BUTTON_HOME;
//	button_buffer[15] |= kpad_buttons & WPAD_PRO_BUTTON_SYNC;
	button_buffer[16] |= kpad_buttons & WPAD_PRO_BUTTON_STICK_L;
	button_buffer[17] |= kpad_buttons & WPAD_PRO_BUTTON_STICK_R;
//	button_buffer[18] |= kpad_buttons & WPAD_PRO_BUTTON_TV;
	button_buffer[19] |= kpad_buttons & WPAD_PRO_BUTTON_LEFT;
	button_buffer[20] |= kpad_buttons & WPAD_PRO_BUTTON_RIGHT;
	button_buffer[21] |= kpad_buttons & WPAD_PRO_BUTTON_UP;
	button_buffer[22] |= kpad_buttons & WPAD_PRO_BUTTON_DOWN;
	button_buffer[23] |= kpad_buttons & WPAD_PRO_STICK_R_EMULATION_LEFT;
	button_buffer[24] |= kpad_buttons & WPAD_PRO_STICK_R_EMULATION_RIGHT;
	button_buffer[25] |= kpad_buttons & WPAD_PRO_STICK_R_EMULATION_UP;
	button_buffer[26] |= kpad_buttons & WPAD_PRO_STICK_R_EMULATION_DOWN;

	return true;
}

const char* ControllerBackend_GetButtonName(unsigned int button_id)
{
	const char *button_names[27] = {
		"Y",
		"B",
		"X",
		"ZL",
		"ZR",
		"A",
		"+",
		"Left Stick Up",
		"Left Stick Down",
		"Left Stick Left",
		"Left Stick Right",
		"L",
		"R",
		"-",
		"Home",
		"Sync",
		"Left Stick Button",
		"Right Stick Button",
		"TV",
		"D-Pad Left",
		"D-Pad Right",
		"D-Pad Up",
		"D-Pad Down",
		"Right Stick Left",
		"Right Stick Right",
		"Right Stick Up",
		"Right Stick Down"
	};

	if (button_id >= sizeof(button_names) / sizeof(button_names[0]))
		return "Unknown";

	return button_names[button_id];
}
