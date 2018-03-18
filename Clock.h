#define DS3234

#ifdef DS3234

#include <SPI.h>

#endif // DS3234





class Clock
{
public:
	Clock(int cs);
	void UpdateTimeDate();
	void SetTime();
	bool IsOraVara(int month, int day, int dayOfWeek);

	int GetYear();
	int GetMonth();
	int GetDay();
	int GetDayOfWeek();
	int GetHour();
	int GetMinute();
	int GetSecond();

	void IncrementYear();
	void IncrementMonth();
	void IncrementDay();
	void IncrementHour();
	void IncrementMinute();
	void IncrementSecond();

private:
	int _cs;

	int _rtcYear = 0;
	int _rtcHour = 0;
	int _rtcMinute = 0;
	int _rtcSecond = 0;
	int _rtcMonth = 0;
	int _rtcDay = 0;
	int _rtcDayOfWeek = 0;
};
