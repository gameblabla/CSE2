#include "../Controller.h"

#include <vpad/input.h>

bool ControllerBackend_Init(void)
{
	VPADInit();

	return true;
}

void ControllerBackend_Deinit(void)
{
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

	VPADStatus vpad_status;
	if (VPADRead(VPAD_CHAN_0, &vpad_status, 1, NULL) == 1)
	{
		//////////////////////////
		// Handle button inputs //
		//////////////////////////

		button_buffer[0] = vpad_status.hold & VPAD_BUTTON_Y;     // Shoot
		button_buffer[1] = vpad_status.hold & VPAD_BUTTON_B;     // Jump
		button_buffer[2] = vpad_status.hold & VPAD_BUTTON_X;     // Map
		button_buffer[3] = vpad_status.hold & VPAD_BUTTON_ZL;    // Weapon left
		button_buffer[4] = vpad_status.hold & VPAD_BUTTON_ZR;    // Weapon right
		button_buffer[5] = vpad_status.hold & VPAD_BUTTON_A;     // Inventory
		button_buffer[6] = vpad_status.hold & VPAD_BUTTON_PLUS;  // Pause
		button_buffer[7] = vpad_status.hold & VPAD_STICK_L_EMULATION_UP;    // Up
		button_buffer[8] = vpad_status.hold & VPAD_STICK_L_EMULATION_DOWN;  // Down
		button_buffer[9] = vpad_status.hold & VPAD_STICK_L_EMULATION_LEFT;  // Left
		button_buffer[10] = vpad_status.hold & VPAD_STICK_L_EMULATION_RIGHT;// Right
		button_buffer[11] = vpad_status.hold & VPAD_BUTTON_L;
		button_buffer[12] = vpad_status.hold & VPAD_BUTTON_R;
		button_buffer[13] = vpad_status.hold & VPAD_BUTTON_MINUS;
		button_buffer[14] = vpad_status.hold & VPAD_BUTTON_HOME;
		button_buffer[15] = vpad_status.hold & VPAD_BUTTON_SYNC;
		button_buffer[17] = vpad_status.hold & VPAD_BUTTON_STICK_L;
		button_buffer[16] = vpad_status.hold & VPAD_BUTTON_STICK_R;
		button_buffer[18] = vpad_status.hold & VPAD_BUTTON_TV;
		button_buffer[23] = vpad_status.hold & VPAD_BUTTON_LEFT;
		button_buffer[24] = vpad_status.hold & VPAD_BUTTON_RIGHT;
		button_buffer[25] = vpad_status.hold & VPAD_BUTTON_UP;
		button_buffer[26] = vpad_status.hold & VPAD_BUTTON_DOWN;
		button_buffer[19] = vpad_status.hold & VPAD_STICK_R_EMULATION_LEFT;
		button_buffer[20] = vpad_status.hold & VPAD_STICK_R_EMULATION_RIGHT;
		button_buffer[21] = vpad_status.hold & VPAD_STICK_R_EMULATION_UP;
		button_buffer[22] = vpad_status.hold & VPAD_STICK_R_EMULATION_DOWN;

		////////////////////////
		// Handle axis inputs //
		////////////////////////

		axis_buffer[0] = (short)(vpad_status.leftStick.x * 0x7FFF);
		axis_buffer[1] = (short)(vpad_status.leftStick.y * -0x7FFF);
		axis_buffer[2] = (short)(vpad_status.rightStick.x * 0x7FFF);
		axis_buffer[3] = (short)(vpad_status.rightStick.y * -0x7FFF);
	}

	return true;
}
