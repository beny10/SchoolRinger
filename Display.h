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
private:
	char _d0;
	char _d1;
	char _d2;
	char _d3;

	char _firstCharacter;
	char _secondCharacter;
	char _thirdCharacter;

	char _latch;
};