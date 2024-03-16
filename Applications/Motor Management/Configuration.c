#include "Configuration.h"

byte GetInjectionMode()
{
#ifdef PWM_IGNITITION
	return INJECTION_DISABLED;
#else
	return TIMER_INJECTION;
#endif

}

bool PwmIgnitionEnabled()
{
#ifdef PWM_IGNITITION
	return TRUE;
#else
	return FALSE;
#endif
}

bool TimerIgnitionEnabled()
{
#ifdef PWM_IGNITITION
	return FALSE;
#else
	return TRUE;
#endif
}

int GetDefaultCogTotal()
{
#ifdef PWM_IGNITITION
	return 2;
#else
	return 60;
#endif
}

int GetDefaultGapSize()
{
#ifdef PWM_IGNITITION
	return 0;
#else
	return 2;
#endif
}

int GetDefaultDeadPointOffset()
{
#ifdef PWM_IGNITITION
	return 0;
#else
	return 20;
#endif
}
int GetDefaultCylinderCount()
{
	return 4;
}

bool IsCogwheelMutable()
{
#ifdef PWM_IGNITITION
	return FALSE;
#else
	return TRUE;
#endif
}
