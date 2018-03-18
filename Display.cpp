#include "Display.h"

Display::Display(char d0,
	char d1,
	char d2,
	char d3,
	char firstCharacter,
	char secondCharacter,
	char thirdCharacter,
	char latch
)
{
	_d0 = d0;
	_d1 = d1;
	_d2 = d2;
	_d3 = d3;

	_firstCharacter = firstCharacter;
	_secondCharacter = secondCharacter;
	_thirdCharacter = thirdCharacter;

	_latch = latch;

	pinMode(_firstCharacter, OUTPUT);
	pinMode(_secondCharacter, OUTPUT);
	pinMode(_thirdCharacter, OUTPUT);

	pinMode(latch, OUTPUT);

	pinMode(_d0, OUTPUT);
	pinMode(_d1, OUTPUT);
	pinMode(_d2, OUTPUT);
	pinMode(_d3, OUTPUT);

	digitalWrite(_firstCharacter, HIGH);
	digitalWrite(_secondCharacter, HIGH);
	digitalWrite(_thirdCharacter, HIGH);
	digitalWrite(_latch, LOW);
}

void Display::PrintDigit(int number)
{
	digitalWrite(_d0, (number & 1));
	digitalWrite(_d1, ((number >> 1) & 1));
	digitalWrite(_d2, ((number >> 2) & 1));
	digitalWrite(_d3, ((number >> 3) & 1));

	digitalWrite(_latch, LOW);
	digitalWrite(_latch, HIGH);
}

#define DISPLAY_REFRESH_RATE 5000

void Display::PrintNumber(int number)
{


	//first digit
	PrintDigit((number / 100) % 10);
	digitalWrite(_firstCharacter, LOW);
	delayMicroseconds(DISPLAY_REFRESH_RATE);

	//second digit
	PrintDigit((number / 10) % 10);
	digitalWrite(_firstCharacter, HIGH);
	digitalWrite(_secondCharacter, LOW);
	delayMicroseconds(DISPLAY_REFRESH_RATE);

	//third digit
	PrintDigit(number % 10);
	digitalWrite(_secondCharacter, HIGH);
	digitalWrite(_thirdCharacter, LOW);
	delayMicroseconds(DISPLAY_REFRESH_RATE);

	digitalWrite(_thirdCharacter, HIGH);
}