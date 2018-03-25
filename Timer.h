#include "Arduino.h"
#include "Display.h"

class Timer
{
public:
	Timer();
	void SetTimer();

	void TurnOn();
	void TurnOff();
	bool IsTimerOn();

	void Delay(unsigned long miliSeconds);
	void Delay2(unsigned long miliSeconds, Display disp);

	void TimerEvent();

	unsigned long GetTotalTimerTicks();
private:
	unsigned long _timerTicks;
};