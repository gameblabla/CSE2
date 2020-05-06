#include "../Controller.h"

// Vanilla Cave Story's controller system is ill-suited for console ports,
// so we emulate a keyboard instead (see `Misc.cpp`).

bool ControllerBackend_Init(void)
{
	return false;
}

void ControllerBackend_Deinit(void)
{
	
}

bool ControllerBackend_GetJoystickStatus(bool **buttons, unsigned int *button_count, short **axes, unsigned int *axis_count)
{
	(void)buttons;
	(void)button_count;
	(void)axes;
	(void)axis_count;

	return false;
}
