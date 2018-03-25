#include "Arduino.h"

class Display
{
public:
	Display(char d0,
		char d1,
		char d2,
		char d3,
		char firstCharacter,
		char secondCharacter,
		char thirdCharacter,
		char latch
		);

	void PrintDigit(int number);
	void PrintNumber(int number);

	void TurnOn();
	void TurnOff();
	bool IsTimerOn();

	void TimerEvent();

	unsigned long GetTotalTimerTicks();
private:
	char _d0;
	char _d1;
	char _d2;
	char _d3;

	char _firstCharacter;
	char _secondCharacter;
	char _thirdCharacter;

	char _latch;

	char _isDisplayOn;
	char _displayContent[3];
	char _currentChar;

	unsigned long _timerTicks;
};