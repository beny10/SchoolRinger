#include "Clock.h"

Clock::Clock(int cs)
{
	_cs = cs;

}

void Clock::InitClock()
{
#ifdef DS3234


	pinMode(_cs, OUTPUT); // chip select
						  // start the SPI library:
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE3); // both mode 1 & 3 should work
								//set control register
	digitalWrite(_cs, LOW);
	SPI.transfer(0x8E);
	SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
	digitalWrite(_cs, HIGH);
	delay(10);

#endif // DS3234
}

void Clock::UpdateTimeDate()
{
#ifdef DS3234

	int TimeDate[7]; //second,minute,hour,null,day,month,year
	for (int i = 0; i <= 6; i++) {
		if (i == 3)
			i++;
		digitalWrite(_cs, LOW);
		SPI.transfer(i + 0x00);
		unsigned int n = SPI.transfer(0x00);
		digitalWrite(_cs, HIGH);

		int a = n & B00001111;
		if (i == 2) {
			int b = (n & B00110000) >> 4; //24 hour mode
			if (b == B00000010)
				b = 20;
			else if (b == B00000001)
				b = 10;
			TimeDate[i] = a + b;
		}
		else if (i == 4) {
			int b = (n & B00110000) >> 4;
			TimeDate[i] = a + b * 10;
		}
		else if (i == 5) {
			int b = (n & B00010000) >> 4;
			TimeDate[i] = a + b * 10;
		}
		else if (i == 6) {
			int b = (n & B11110000) >> 4;
			TimeDate[i] = a + b * 10;
		}
		else {
			int b = (n & B01110000) >> 4;
			TimeDate[i] = a + b * 10;
		}
	}
	_rtcYear = TimeDate[6];
	_rtcMonth = TimeDate[5];
	_rtcDay = TimeDate[4];
	_rtcHour = TimeDate[2];
	_rtcMinute = TimeDate[1];
	_rtcSecond = TimeDate[0];
	//RtcDayOfWeek = getDayOfWeek();

#endif // DS3234
	/*Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0); // set DS3231 register pointer to 00h
	Wire.endTransmission();
	Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
	// request seven bytes of data from DS3231 starting from register 00h
	RtcSecond = bcdToDec(Wire.read() & 0x7f);
	RtcMinute = bcdToDec(Wire.read());
	RtcHour = bcdToDec(Wire.read() & 0x3f);
	RtcDayOfWeek = bcdToDec(Wire.read());
	RtcDay = bcdToDec(Wire.read());
	RtcMonth = bcdToDec(Wire.read());
	RtcYear = bcdToDec(Wire.read());
	if (isOraVara(RtcMonth, RtcDay, RtcDayOfWeek))
	{
	RtcHour++;
	if (RtcHour == 24)
	RtcHour = 0;
	}*/
}

void Clock::SetTime()
{
#ifdef DS3234

	int TimeDate[7] = { _rtcSecond, _rtcMinute, _rtcHour, 
		0, _rtcDay, _rtcMonth, _rtcYear };
	for (int i = 0; i <= 6; i++) {
		if (i == 3)
			i++;
		int b = TimeDate[i] / 10;
		int a = TimeDate[i] - b * 10;
		if (i == 2) {
			if (b == 2)
				b = B00000010;
			else if (b == 1)
				b = B00000001;
		}
		TimeDate[i] = a + (b << 4);

		digitalWrite(_cs, LOW);
		SPI.transfer(i + 0x80);
		SPI.transfer(TimeDate[i]);
		digitalWrite(_cs, HIGH);
	}

#endif // DS3234


	// sets time and date data to DS3231
	/* Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0); // set next input to start at the seconds register
	Wire.write(decToBcd(second)); // set seconds
	Wire.write(decToBcd(minute)); // set minutes
	if (isOraVara(RtcMonth, RtcDay, RtcDayOfWeek))
	{
	if (hour > 0)
	hour--;
	else
	hour = 23;
	}
	Serial.print("hour=");
	Serial.println(hour);
	Wire.write(decToBcd(hour)); // set hours
	Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
	Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
	Wire.write(decToBcd(month)); // set month
	Wire.write(decToBcd(year)); // set year (0 to 99)
	Wire.endTransmission();*/
}

bool Clock::IsOraVara(int month, int day, int dayOfWeek)
{
	bool isOraVara = false;
	if (month > 3 && month < 10)
	{
		isOraVara = true;
	}
	else if (month == 3)
	{
		isOraVara = false;
		if (day > 24)
		{
			isOraVara = true;
			if (day + (7 - dayOfWeek) <= 31)
			{
				isOraVara = false;
			}
		}
	}
	else if (month == 10)
	{
		isOraVara = true;
		if (day > 24)
		{
			isOraVara = false;
			if (day + (7 - dayOfWeek) <= 31)
			{
				isOraVara = true;
			}
		}
	}
	return isOraVara;
}

int Clock::GetYear()
{
	return _rtcYear;
}

int Clock::GetMonth()
{
	return _rtcMonth;
}

int Clock::GetDay()
{
	return _rtcDay;
}

int Clock::GetDayOfWeek()
{
	return _rtcDayOfWeek;
}

int Clock::GetHour()
{
	return _rtcHour;
}

int Clock::GetMinute()
{
	return _rtcMinute;
}

int Clock::GetSecond()
{
	return _rtcSecond;
}

void Clock::IncrementYear()
{
	_rtcYear++;
	if (99 < _rtcYear)
	{
		_rtcYear = 0;
	}
}

void Clock::IncrementMonth()
{
	_rtcMonth++;
	if (12 < _rtcMonth)
	{
		_rtcMonth = 1;
	}
}

void Clock::IncrementDay()
{
	_rtcDay++;
	if (31 < _rtcDay)
	{
		_rtcDay = 0;
	}
}

void Clock::IncrementHour()
{
	_rtcHour++;
	if (23 < _rtcHour)
	{
		_rtcHour = 0;
	}
}

void Clock::IncrementMinute()
{
	_rtcMinute++;
	if (59 < _rtcMinute)
	{
		_rtcMinute = 0;
	}
}

void Clock::IncrementSecond()
{
	_rtcSecond++;
	if (59 < _rtcSecond)
	{
		_rtcSecond = 0;
	}
}
