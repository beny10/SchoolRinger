#include "Timer.h"

Timer::Timer()
{

}

void Timer::SetTimer()
{
	//stop interrupts
	cli();

	// set entire TCCR2A register to 0
	TCCR0A = 0;
	// same for TCCR2B
	TCCR0B = 0;
	//initialize counter value to 0
	TCNT0 = 0;
	// set compare match register for 2khz increments
	// frequency will be 124.5Hz
	//timer trigger every 249 Hz
	OCR0A = 250;
	// turn on CTC mode
	TCCR0A |= (1 << WGM01);
	// Set CS01 and CS00 bits for 64 prescaler
	TCCR0B |= (1 << CS02);

	TurnOff();

	//allow interrupts
	sei();
}

void Timer::TurnOn()
{
	//enable timer compare interrupt
	TIMSK0 |= (1 << OCIE0A);
}

void Timer::TurnOff()
{
	//disable timer compare interrupt
	TIMSK0 &= 0xFF ^ (1 << OCIE0A);

	_timerTicks = 0;
}

bool Timer::IsTimerOn()
{
	return (0 != (TIMSK0 & (1 << OCIE0A)));
}


void Timer::TimerEvent()
{
	//every 4 miliseconds
	_timerTicks++;
}

unsigned long Timer::GetTotalTimerTicks()
{
	return _timerTicks;
}

void Timer::Delay(unsigned long miliSeconds)
{
	_timerTicks = 0;

	unsigned long start = _timerTicks;
	

	while ((_timerTicks - start) < (miliSeconds >> 2))
	{
		//digitalWrite(5, digitalRead(5) ^ 1);
		//Serial.print("-");
		///Serial.print("asd:");
		//Serial.print(_timerTicks / 10000);
		//Serial.println(_timerTicks % 10000);
		//wait
	}

}

void Timer::Delay2(unsigned long miliSeconds, Display disp)
{
	_timerTicks = 0;

	unsigned long start = GetTotalTimerTicks();
	digitalWrite(5, digitalRead(5) ^ 1);

	while (GetTotalTimerTicks() < 10)
	{
		
		//disp.PrintNumber(miliSeconds);
		//Serial.print("-");
		///Serial.print("asd:");
		//Serial.print(_timerTicks / 10000);
		//Serial.println(_timerTicks % 10000);
		//wait
	}

}