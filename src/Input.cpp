#include "Input.h"

#include "Backends/Controller.h"

#include "WindowsWrapper.h"

void ReleaseDirectInput(void)
{
	ControllerBackend_Deinit();
}

BOOL InitDirectInput(void)
{
	return ControllerBackend_Init();
}

BOOL GetJoystickStatus(JOYSTICK_STATUS *status)
{
	return ControllerBackend_GetJoystickStatus(status);
}

BOOL ResetJoystickStatus(void)
{
	return ControllerBackend_ResetJoystickStatus();
}
