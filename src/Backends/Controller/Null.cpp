// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Controller.h"

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
