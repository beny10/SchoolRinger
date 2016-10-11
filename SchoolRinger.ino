#include <SPI.h>
const int  cs = 9;
#define SOUND_INTERVAL 2000
#define RELAY_PIN 6
#include <virtuabotixRTC.h>
int ringingTime[][3] = {{8, 0, 0}, {8, 50, 0}, {9, 0, 0}, {9, 50, 0}, {10, 0, 0}, {10, 50, 0}, {11, 10, 0}, {12, 0, 0}, {12, 10, 0}, {13, 0, 0}, {13, 10, 0}, {13, 59, 0}};//13:59 "sa prinda elevii conventia"
//int ringingTime[][3]={{21,30,00},{21,32,0}};
int ringingCount = 12;
int statusLedPin = 8;
int RtcYear = 0;
int RtcHour = 0;
int RtcMinute = 0;
int RtcSecond = 0;
int RtcMonth = 0;
int RtcDay = 0;
int RtcDayOfWeek = 0;
int RTC_init()
{
  pinMode(cs, OUTPUT); // chip select
  // start the SPI library:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3); // both mode 1 & 3 should work
  //set control register
  digitalWrite(cs, LOW);
  SPI.transfer(0x8E);
  SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
  digitalWrite(cs, HIGH);
  delay(10);
}
int getDayOfWeek()
{
  int dayOfWeek = 0;
  int savedYear = 16;
  int savedDaysOfMonths[] = { 5, 1, 2, 5, 7, 3, 5, 1, 4, 6, 2, 4 };
  //the first day of 2016 was on friday
  int passedYears = RtcYear - savedYear;
  int dephasedDays = passedYears;
  int leapYears = 0;
  for (int i = savedYear + 1; i <= RtcYear; ++i)
  {
    if (i % 4 == 0)
      leapYears++;
  }
  dephasedDays += leapYears;
  if (RtcMonth < 3)
  {
    if (savedYear % 4 == 0)
      dephasedDays++;
    if (RtcYear % 4 == 0)
      dephasedDays--;
    if (RtcYear == savedYear)
    {
      dephasedDays = 0;
    }
  }
  dayOfWeek = savedDaysOfMonths[RtcMonth - 1] + dephasedDays;
  dayOfWeek += (RtcDay - 1);
  dayOfWeek = dayOfWeek % 7;
  if (dayOfWeek == 0)
    dayOfWeek = 7;
  return dayOfWeek;
}
void updateTimeDate() {
  int TimeDate [7]; //second,minute,hour,null,day,month,year
  for (int i = 0; i <= 6; i++) {
    if (i == 3)
      i++;
    digitalWrite(cs, LOW);
    SPI.transfer(i + 0x00);
    unsigned int n = SPI.transfer(0x00);
    digitalWrite(cs, HIGH);
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
  RtcYear = TimeDate[6];
  RtcMonth = TimeDate[5];
  RtcDay = TimeDate[4];
  RtcHour = TimeDate[2];
  RtcMinute = TimeDate[1];
  RtcSecond = TimeDate[0];
  RtcDayOfWeek = getDayOfWeek();
}
void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(statusLedPin, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.begin(9600);
  RTC_init();
}
bool isOraVara(int month, int day, int dayOfWeek)
{
  bool isOraVara = false;
  if (month > 3 && month < 10)
  {
    isOraVara = true;
  }
  else if (month == 3)
  {
    isOraVara=false;
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
    isOraVara=true;
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
void Ring()
{
  digitalWrite(RELAY_PIN, HIGH);
  delay(SOUND_INTERVAL);
  digitalWrite(RELAY_PIN, LOW);
}
void loop()
{
  updateTimeDate();
  if (isOraVara(RtcMonth, RtcDay, RtcDayOfWeek))
  {
    RtcHour++;
    Serial.print(1);
  }
  else
  {
    Serial.print(0);
  }
  Serial.print(" ");
  Serial.print(RtcYear);
  Serial.print("/");
  Serial.print(RtcMonth);
  Serial.print("/");
  Serial.print(RtcDay);
  Serial.print(" ");
  Serial.print(RtcDayOfWeek);
  Serial.print("    ");
  Serial.print(RtcHour);
  Serial.print(" ");
  Serial.print(RtcMinute);
  Serial.print(" ");
  Serial.println(RtcSecond);
  if (RtcYear < 16 || RtcYear > 25)
  {
    digitalWrite(statusLedPin, HIGH);
    delay(300);
    digitalWrite(statusLedPin, LOW);
    delay(300);
    return;
  }
  if (RtcSecond % 2 == 0)
  {
    digitalWrite(statusLedPin, HIGH);
  }
  else
  {
    digitalWrite(statusLedPin, LOW);
  }
  if (RtcDayOfWeek == 6 || RtcDayOfWeek == 7)
  {
    //delay(100000);
    return;
  }
  for (int i = 0; i < ringingCount; ++i)
  {
    if (RtcHour == ringingTime[i][0] && RtcMinute == ringingTime[i][1] && RtcSecond >= ringingTime[i][2] - 1 && RtcSecond <= ringingTime[i][2] + 1)
    {
      Serial.println("Sun");
      Ring();
    }
  }
  delay(1000);
}
